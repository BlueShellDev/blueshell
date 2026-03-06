#pragma once

#include <optional>
#include <string>

namespace BS::Plugins
{
    struct EnumClassHash
    {
        template <typename T>
        size_t operator()(T value) const
        {
            return static_cast<size_t>(value);
        }
    };

    enum class PluginPermission
    {
        EditorContextRead = 0,
        LevelEditWrite,
        HostLogWrite
    };

    enum class PluginApiName
    {
        RequestEditorContext = 0,
        RequestLevelEdit,
        HostLog
    };

    enum class PluginStatus
    {
        Discovered = 0,
        InvalidManifest,
        Disabled,
        LoadError,
        Loaded,
        RuntimeFaulted
    };

    const char *ToString(PluginPermission permission);
    const char *ToString(PluginApiName api);
    const char *ToString(PluginStatus status);

    std::optional<PluginPermission> ParsePermission(const std::string &permission_name);
    std::optional<PluginApiName> ParseApiName(const std::string &api_name);

    PluginPermission RequiredPermissionForApi(PluginApiName api_name);
};
