#pragma once

#include <filesystem>
#include <string>
#include <unordered_set>
#include <vector>

#include "plugin_api.h"
#include "plugin_types.h"

namespace BS::Plugins
{
    struct PluginManifest
    {
        std::string id;
        std::string name;
        std::string version;

        uint32_t apiVersion{0};

        std::filesystem::path pluginDirectory;
        std::filesystem::path entryRelativePath;
        std::filesystem::path entryAbsolutePath;

        std::vector<PluginApiName> declaredApis;
        std::vector<PluginPermission> declaredPermissions;

        std::unordered_set<PluginApiName, EnumClassHash> declaredApiSet;
        std::unordered_set<PluginPermission, EnumClassHash> declaredPermissionSet;
    };

    struct PluginManifestLoadResult
    {
        bool ok{false};
        PluginManifest manifest;
        std::string error;
    };

    PluginManifestLoadResult LoadPluginManifestFromPath(const std::filesystem::path &plugin_yaml_path);
};
