#include "plugin_types.h"

namespace
{
    using BS::Plugins::PluginApiName;
    using BS::Plugins::PluginPermission;
    using BS::Plugins::PluginStatus;
}

const char *BS::Plugins::ToString(PluginPermission permission)
{
    switch (permission)
    {
        case PluginPermission::EditorContextRead:
            return "editor.context.read";
        case PluginPermission::LevelEditWrite:
            return "level.edit.write";
        case PluginPermission::HostLogWrite:
            return "host.log.write";
        default:
            return "unknown.permission";
    }
}

const char *BS::Plugins::ToString(PluginApiName api)
{
    switch (api)
    {
        case PluginApiName::RequestEditorContext:
            return "RequestEditorContext";
        case PluginApiName::RequestLevelEdit:
            return "RequestLevelEdit";
        case PluginApiName::HostLog:
            return "HostLog";
        default:
            return "UnknownApi";
    }
}

const char *BS::Plugins::ToString(PluginStatus status)
{
    switch (status)
    {
        case PluginStatus::Discovered:
            return "Discovered";
        case PluginStatus::InvalidManifest:
            return "InvalidManifest";
        case PluginStatus::Disabled:
            return "Disabled";
        case PluginStatus::LoadError:
            return "LoadError";
        case PluginStatus::Loaded:
            return "Loaded";
        case PluginStatus::RuntimeFaulted:
            return "RuntimeFaulted";
        default:
            return "UnknownStatus";
    }
}

std::optional<PluginPermission> BS::Plugins::ParsePermission(const std::string &permission_name)
{
    if (permission_name == "editor.context.read")
        return PluginPermission::EditorContextRead;
    if (permission_name == "level.edit.write")
        return PluginPermission::LevelEditWrite;
    if (permission_name == "host.log.write")
        return PluginPermission::HostLogWrite;

    return std::nullopt;
}

std::optional<PluginApiName> BS::Plugins::ParseApiName(const std::string &api_name)
{
    if (api_name == "RequestEditorContext")
        return PluginApiName::RequestEditorContext;
    if (api_name == "RequestLevelEdit")
        return PluginApiName::RequestLevelEdit;
    if (api_name == "HostLog")
        return PluginApiName::HostLog;

    return std::nullopt;
}

PluginPermission BS::Plugins::RequiredPermissionForApi(PluginApiName api_name)
{
    switch (api_name)
    {
        case PluginApiName::RequestEditorContext:
            return PluginPermission::EditorContextRead;
        case PluginApiName::RequestLevelEdit:
            return PluginPermission::LevelEditWrite;
        case PluginApiName::HostLog:
            return PluginPermission::HostLogWrite;
        default:
            return PluginPermission::HostLogWrite;
    }
}
