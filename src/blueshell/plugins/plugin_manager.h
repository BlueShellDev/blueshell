#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "plugin_runtime.h"

namespace BS::Plugins
{
    struct PluginRecord
    {
        std::string id;
        std::filesystem::path directoryPath;
        std::optional<PluginManifest> manifest;

        PluginGrants grants;
        PluginRuntime runtime;
        PluginDiagnostics diagnostics;

        PluginStatus status{PluginStatus::Discovered};
        std::string statusMessage;
    };

    class PluginManager
    {
    public:
        bool Initialize(
            const std::filesystem::path &plugins_root,
            EditorStateProvider *editor_state_provider,
            LevelEditSink *level_edit_sink);

        void Shutdown();

        bool DiscoverPlugins(std::string *out_error = nullptr);
        bool ImportPluginBundle(const std::filesystem::path &source_folder, std::string *out_error);

        void TickAll(double delta_seconds);

        bool SetPluginEnabled(const std::string &plugin_id, bool enabled, std::string *out_error);
        bool SetPermission(const std::string &plugin_id, PluginPermission permission, bool allow, std::string *out_error);
        bool SetDebugOverride(const std::string &plugin_id, bool allow, std::string *out_error);

        bool LoadPlugin(const std::string &plugin_id, std::string *out_error);
        bool UnloadPlugin(const std::string &plugin_id, std::string *out_error);

        std::vector<std::string> GetPluginIdsSorted() const;
        const PluginRecord *FindPlugin(const std::string &plugin_id) const;
        PluginRecord *FindPluginMutable(const std::string &plugin_id);

        const std::filesystem::path &GetPluginsRoot() const;

    private:
        bool PersistGrants(PluginRecord &record, std::string *out_error);
        bool LoadPluginInternal(PluginRecord &record, std::string *out_error);
        void UnloadPluginInternal(PluginRecord &record);
        std::filesystem::path GrantsPathFor(const PluginRecord &record) const;

        std::filesystem::path pluginsRoot_;
        HostApiServices hostApiServices_;
        std::unordered_map<std::string, PluginRecord> plugins_;
    };
};
