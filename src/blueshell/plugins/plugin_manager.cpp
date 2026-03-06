#include "plugin_manager.h"

#include <algorithm>
#include <exception>
#include <format>
#include <thread>

#include "core/console.h"

namespace
{
    std::string BuildPluginKey(const BS::Plugins::PluginManifestLoadResult &manifest_result, const std::filesystem::path &directory)
    {
        if (manifest_result.ok)
            return manifest_result.manifest.id;

        return directory.filename().string();
    }
}

bool BS::Plugins::PluginManager::Initialize(
    const std::filesystem::path &plugins_root,
    EditorStateProvider *editor_state_provider,
    LevelEditSink *level_edit_sink)
{
    pluginsRoot_ = plugins_root;
    hostApiServices_.editorStateProvider = editor_state_provider;
    hostApiServices_.levelEditSink = level_edit_sink;
    hostApiServices_.mainThreadId = std::this_thread::get_id();

    std::string error;
    if (!DiscoverPlugins(&error))
    {
        Core::Console::Global().Err("[Plugin] Discovery failed during initialization: {}", error);
        return false;
    }

    return true;
}

void BS::Plugins::PluginManager::Shutdown()
{
    for (auto &[plugin_id, record] : plugins_)
    {
        (void)plugin_id;
        UnloadPluginInternal(record);
    }
}

bool BS::Plugins::PluginManager::DiscoverPlugins(std::string *out_error)
{
    Shutdown();
    plugins_.clear();

    try
    {
        std::filesystem::create_directories(pluginsRoot_);

        for (const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(pluginsRoot_))
        {
            if (!entry.is_directory())
                continue;

            const std::filesystem::path plugin_dir = entry.path();
            const std::filesystem::path plugin_manifest_path = plugin_dir / "plugin.yaml";
            if (!std::filesystem::exists(plugin_manifest_path))
                continue;

            const PluginManifestLoadResult manifest_result = LoadPluginManifestFromPath(plugin_manifest_path);
            const std::string plugin_id = manifest_result.ok ? manifest_result.manifest.id : BuildPluginKey(manifest_result, plugin_dir);

            auto [it, inserted] = plugins_.try_emplace(plugin_id);
            if (!inserted)
            {
                // Keep first-discovered plugin if duplicate IDs exist.
                Core::Console::Global().Warn("[Plugin] Duplicate plugin id '{}' at '{}'; skipping duplicate", plugin_id, plugin_dir.string());
                continue;
            }

            PluginRecord &record = it->second;
            record.directoryPath = plugin_dir;
            record.id = plugin_id;

            if (!manifest_result.ok)
            {
                record.status = PluginStatus::InvalidManifest;
                record.statusMessage = manifest_result.error;
                continue;
            }

            record.manifest = manifest_result.manifest;
            record.status = PluginStatus::Discovered;
            record.statusMessage.clear();

            std::string grants_error;
            const std::filesystem::path grants_path = GrantsPathFor(record);
            if (std::filesystem::exists(grants_path))
            {
                if (!LoadPluginGrants(grants_path, record.manifest.value(), &record.grants, &grants_error))
                {
                    record.status = PluginStatus::InvalidManifest;
                    record.statusMessage = std::format("Invalid grants file: {}", grants_error);
                    continue;
                }
            }
            else
            {
                record.grants = BuildDefaultGrants(record.manifest.value());
                if (!PersistGrants(record, &grants_error))
                {
                    record.status = PluginStatus::InvalidManifest;
                    record.statusMessage = std::format("Failed to create default grants file: {}", grants_error);
                    continue;
                }
            }

            if (!record.grants.enabled)
            {
                record.status = PluginStatus::Disabled;
                record.statusMessage = "Plugin is disabled";
                continue;
            }

            if (!LoadPluginInternal(record, &record.statusMessage))
                record.status = PluginStatus::LoadError;
        }

        return true;
    }
    catch (const std::exception &ex)
    {
        if (out_error != nullptr)
            *out_error = std::format("DiscoverPlugins failed: {}", ex.what());
        return false;
    }
}

bool BS::Plugins::PluginManager::ImportPluginBundle(const std::filesystem::path &source_folder, std::string *out_error)
{
    try
    {
        if (source_folder.empty() || !std::filesystem::exists(source_folder) || !std::filesystem::is_directory(source_folder))
        {
            if (out_error != nullptr)
                *out_error = "Import source path is not a directory";
            return false;
        }

        const std::filesystem::path source_manifest = source_folder / "plugin.yaml";
        const PluginManifestLoadResult manifest_result = LoadPluginManifestFromPath(source_manifest);
        if (!manifest_result.ok)
        {
            if (out_error != nullptr)
                *out_error = std::format("Manifest is invalid: {}", manifest_result.error);
            return false;
        }

        const std::filesystem::path destination = pluginsRoot_ / manifest_result.manifest.id;
        if (std::filesystem::exists(destination))
        {
            if (out_error != nullptr)
                *out_error = std::format("Plugin '{}' is already installed", manifest_result.manifest.id);
            return false;
        }

        std::filesystem::create_directories(pluginsRoot_);
        std::filesystem::copy(source_folder, destination, std::filesystem::copy_options::recursive);

        const PluginManifestLoadResult destination_manifest = LoadPluginManifestFromPath(destination / "plugin.yaml");
        if (!destination_manifest.ok)
        {
            if (out_error != nullptr)
                *out_error = std::format("Copied plugin manifest is invalid: {}", destination_manifest.error);
            return false;
        }

        PluginGrants grants = BuildDefaultGrants(destination_manifest.manifest);
        std::string grants_error;
        if (!SavePluginGrants(destination / "grants.yaml", destination_manifest.manifest, grants, &grants_error))
        {
            if (out_error != nullptr)
                *out_error = std::format("Failed to write grants.yaml: {}", grants_error);
            return false;
        }

        return DiscoverPlugins(out_error);
    }
    catch (const std::exception &ex)
    {
        if (out_error != nullptr)
            *out_error = std::format("Import failed: {}", ex.what());
        return false;
    }
}

void BS::Plugins::PluginManager::TickAll(double delta_seconds)
{
    for (auto &[plugin_id, record] : plugins_)
    {
        (void)plugin_id;

        if (record.status != PluginStatus::Loaded)
            continue;

        std::string error;
        if (!record.runtime.Tick(delta_seconds, &error))
        {
            record.runtime.Unload();
            record.status = PluginStatus::RuntimeFaulted;
            record.statusMessage = error.empty() ? "Plugin tick failed" : error;
            Core::Console::Global().Err("[Plugin] {}", record.statusMessage);
        }
    }
}

bool BS::Plugins::PluginManager::SetPluginEnabled(const std::string &plugin_id, bool enabled, std::string *out_error)
{
    PluginRecord *record = FindPluginMutable(plugin_id);
    if (record == nullptr || !record->manifest.has_value())
    {
        if (out_error != nullptr)
            *out_error = std::format("Plugin '{}' not found", plugin_id);
        return false;
    }

    record->grants.enabled = enabled;
    if (!PersistGrants(*record, out_error))
        return false;

    if (!enabled)
    {
        UnloadPluginInternal(*record);
        record->status = PluginStatus::Disabled;
        record->statusMessage = "Plugin is disabled";
        return true;
    }

    if (!LoadPluginInternal(*record, out_error))
    {
        record->status = PluginStatus::LoadError;
        return false;
    }

    return true;
}

bool BS::Plugins::PluginManager::SetPermission(
    const std::string &plugin_id,
    PluginPermission permission,
    bool allow,
    std::string *out_error)
{
    PluginRecord *record = FindPluginMutable(plugin_id);
    if (record == nullptr || !record->manifest.has_value())
    {
        if (out_error != nullptr)
            *out_error = std::format("Plugin '{}' not found", plugin_id);
        return false;
    }

    if (!record->manifest->declaredPermissionSet.contains(permission))
    {
        if (out_error != nullptr)
            *out_error = std::format("Plugin '{}' did not declare permission '{}'", plugin_id, ToString(permission));
        return false;
    }

    record->grants.permissions[permission] = allow;
    if (!PersistGrants(*record, out_error))
        return false;

    return true;
}

bool BS::Plugins::PluginManager::SetDebugOverride(const std::string &plugin_id, bool allow, std::string *out_error)
{
    PluginRecord *record = FindPluginMutable(plugin_id);
    if (record == nullptr || !record->manifest.has_value())
    {
        if (out_error != nullptr)
            *out_error = std::format("Plugin '{}' not found", plugin_id);
        return false;
    }

    if (!IsDebugUndeclaredApiOverrideSupported() && allow)
    {
        if (out_error != nullptr)
            *out_error = "Debug override is not supported in release builds";
        return false;
    }

    record->grants.allowUndeclaredApisForDebug = allow;
    if (!PersistGrants(*record, out_error))
        return false;

    return true;
}

bool BS::Plugins::PluginManager::LoadPlugin(const std::string &plugin_id, std::string *out_error)
{
    PluginRecord *record = FindPluginMutable(plugin_id);
    if (record == nullptr)
    {
        if (out_error != nullptr)
            *out_error = std::format("Plugin '{}' not found", plugin_id);
        return false;
    }

    if (!record->manifest.has_value())
    {
        if (out_error != nullptr)
            *out_error = std::format("Plugin '{}' manifest is invalid", plugin_id);
        return false;
    }

    if (!record->grants.enabled)
    {
        if (out_error != nullptr)
            *out_error = std::format("Plugin '{}' is disabled", plugin_id);
        return false;
    }

    return LoadPluginInternal(*record, out_error);
}

bool BS::Plugins::PluginManager::UnloadPlugin(const std::string &plugin_id, std::string *out_error)
{
    PluginRecord *record = FindPluginMutable(plugin_id);
    if (record == nullptr)
    {
        if (out_error != nullptr)
            *out_error = std::format("Plugin '{}' not found", plugin_id);
        return false;
    }

    UnloadPluginInternal(*record);
    record->status = record->grants.enabled ? PluginStatus::Discovered : PluginStatus::Disabled;
    record->statusMessage = record->grants.enabled ? "Plugin unloaded" : "Plugin is disabled";
    return true;
}

std::vector<std::string> BS::Plugins::PluginManager::GetPluginIdsSorted() const
{
    std::vector<std::string> ids;
    ids.reserve(plugins_.size());

    for (const auto &[id, record] : plugins_)
    {
        (void)record;
        ids.push_back(id);
    }

    std::sort(ids.begin(), ids.end());
    return ids;
}

const BS::Plugins::PluginRecord *BS::Plugins::PluginManager::FindPlugin(const std::string &plugin_id) const
{
    const auto it = plugins_.find(plugin_id);
    if (it == plugins_.end())
        return nullptr;
    return &it->second;
}

BS::Plugins::PluginRecord *BS::Plugins::PluginManager::FindPluginMutable(const std::string &plugin_id)
{
    const auto it = plugins_.find(plugin_id);
    if (it == plugins_.end())
        return nullptr;
    return &it->second;
}

const std::filesystem::path &BS::Plugins::PluginManager::GetPluginsRoot() const
{
    return pluginsRoot_;
}

bool BS::Plugins::PluginManager::PersistGrants(PluginRecord &record, std::string *out_error)
{
    if (!record.manifest.has_value())
    {
        if (out_error != nullptr)
            *out_error = std::format("Cannot save grants: plugin '{}' has invalid manifest", record.id);
        return false;
    }

    return SavePluginGrants(GrantsPathFor(record), record.manifest.value(), record.grants, out_error);
}

bool BS::Plugins::PluginManager::LoadPluginInternal(PluginRecord &record, std::string *out_error)
{
    if (!record.manifest.has_value())
    {
        if (out_error != nullptr)
            *out_error = "Cannot load plugin with invalid manifest";
        return false;
    }

    record.diagnostics = {};

    std::string error;
    if (!record.runtime.Load(&record.manifest.value(), &record.grants, &hostApiServices_, &record.diagnostics, &error))
    {
        record.status = PluginStatus::LoadError;
        record.statusMessage = error;

        if (out_error != nullptr)
            *out_error = error;

        Core::Console::Global().Err("[Plugin] Failed to load '{}': {}", record.id, error);
        return false;
    }

    record.status = PluginStatus::Loaded;
    record.statusMessage = "Loaded";
    Core::Console::Global().Log("[Plugin] Loaded '{}'", record.id);
    return true;
}

void BS::Plugins::PluginManager::UnloadPluginInternal(PluginRecord &record)
{
    if (!record.runtime.IsLoaded())
        return;

    record.runtime.Unload();
    Core::Console::Global().Log("[Plugin] Unloaded '{}'", record.id);
}

std::filesystem::path BS::Plugins::PluginManager::GrantsPathFor(const PluginRecord &record) const
{
    return record.directoryPath / "grants.yaml";
}
