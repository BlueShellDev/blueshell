#include "plugin_grants.h"

#include <exception>
#include <fstream>
#include <format>

#include "yaml-cpp/yaml.h"

bool BS::Plugins::IsDebugUndeclaredApiOverrideSupported()
{
#if defined(NDEBUG)
    return false;
#else
    return true;
#endif
}

BS::Plugins::PluginGrants BS::Plugins::BuildDefaultGrants(const PluginManifest &manifest)
{
    PluginGrants grants;
    grants.enabled = false;
    grants.allowUndeclaredApisForDebug = false;

    for (const PluginPermission permission : manifest.declaredPermissions)
        grants.permissions[permission] = false;

    return grants;
}

bool BS::Plugins::LoadPluginGrants(
    const std::filesystem::path &grants_path,
    const PluginManifest &manifest,
    PluginGrants *out_grants,
    std::string *out_error)
{
    if (out_grants == nullptr)
    {
        if (out_error != nullptr)
            *out_error = "LoadPluginGrants called with out_grants == nullptr";
        return false;
    }

    PluginGrants grants = BuildDefaultGrants(manifest);

    if (!std::filesystem::exists(grants_path))
    {
        *out_grants = grants;
        return true;
    }

    try
    {
        const YAML::Node root = YAML::LoadFile(grants_path.string());

        if (root["enabled"])
            grants.enabled = root["enabled"].as<bool>();

        if (root["permissions"])
        {
            if (!root["permissions"].IsMap())
            {
                if (out_error != nullptr)
                    *out_error = "grants.yaml key 'permissions' must be a mapping";
                return false;
            }

            for (const auto &entry : root["permissions"])
            {
                const std::string permission_name = entry.first.as<std::string>();
                const std::string permission_value = entry.second.as<std::string>();

                const auto parsed = ParsePermission(permission_name);
                if (!parsed.has_value())
                {
                    if (out_error != nullptr)
                        *out_error = std::format("grants.yaml includes unknown permission '{}'", permission_name);
                    return false;
                }

                if (!manifest.declaredPermissionSet.contains(parsed.value()))
                {
                    if (out_error != nullptr)
                    {
                        *out_error = std::format(
                            "grants.yaml includes permission '{}' not declared by plugin",
                            permission_name);
                    }
                    return false;
                }

                if (permission_value == "allow")
                    grants.permissions[parsed.value()] = true;
                else if (permission_value == "deny")
                    grants.permissions[parsed.value()] = false;
                else
                {
                    if (out_error != nullptr)
                        *out_error = std::format(
                            "Invalid permission value '{}' for '{}' (expected allow|deny)",
                            permission_value,
                            permission_name);
                    return false;
                }
            }
        }

        if (root["allowUndeclaredApisForDebug"])
            grants.allowUndeclaredApisForDebug = root["allowUndeclaredApisForDebug"].as<bool>();

        *out_grants = grants;
        return true;
    }
    catch (const YAML::Exception &ex)
    {
        if (out_error != nullptr)
            *out_error = std::format("Failed to parse grants yaml '{}': {}", grants_path.string(), ex.what());
        return false;
    }
    catch (const std::exception &ex)
    {
        if (out_error != nullptr)
            *out_error = std::format("Failed to load grants '{}': {}", grants_path.string(), ex.what());
        return false;
    }
}

bool BS::Plugins::SavePluginGrants(
    const std::filesystem::path &grants_path,
    const PluginManifest &manifest,
    const PluginGrants &grants,
    std::string *out_error)
{
    try
    {
        YAML::Node root;
        root["enabled"] = grants.enabled;

        YAML::Node permissions_node;
        for (const PluginPermission permission : manifest.declaredPermissions)
            permissions_node[ToString(permission)] = IsPermissionGranted(grants, permission) ? "allow" : "deny";

        root["permissions"] = permissions_node;

        if (IsDebugUndeclaredApiOverrideSupported())
            root["allowUndeclaredApisForDebug"] = grants.allowUndeclaredApisForDebug;

        std::filesystem::create_directories(grants_path.parent_path());

        std::ofstream out_file(grants_path);
        if (!out_file)
        {
            if (out_error != nullptr)
                *out_error = std::format("Failed to open '{}' for writing", grants_path.string());
            return false;
        }

        out_file << root;
        return true;
    }
    catch (const std::exception &ex)
    {
        if (out_error != nullptr)
            *out_error = std::format("Failed to write grants '{}': {}", grants_path.string(), ex.what());
        return false;
    }
}
