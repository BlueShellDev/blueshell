#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

#include "plugin_manifest.h"

namespace BS::Plugins
{
    struct PluginGrants
    {
        bool enabled{false};
        std::unordered_map<PluginPermission, bool, EnumClassHash> permissions;
        bool allowUndeclaredApisForDebug{false};
    };

    bool IsDebugUndeclaredApiOverrideSupported();

    PluginGrants BuildDefaultGrants(const PluginManifest &manifest);
    bool LoadPluginGrants(
        const std::filesystem::path &grants_path,
        const PluginManifest &manifest,
        PluginGrants *out_grants,
        std::string *out_error);
    bool SavePluginGrants(
        const std::filesystem::path &grants_path,
        const PluginManifest &manifest,
        const PluginGrants &grants,
        std::string *out_error);

    inline bool IsPermissionGranted(const PluginGrants &grants, PluginPermission permission)
    {
        const auto it = grants.permissions.find(permission);
        if (it == grants.permissions.end())
            return false;
        return it->second;
    }
};
