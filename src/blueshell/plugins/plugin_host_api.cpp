#include "plugin_host_api.h"

#include <cstdio>
#include <cstring>
#include <format>

namespace
{
    using BS::Plugins::HostApiContext;
    using BS::Plugins::PluginApiName;
    using BS::Plugins::PluginPermission;

    void RegisterDeniedCall(HostApiContext *context, PluginApiName api_name, const std::string &reason)
    {
        if (context == nullptr || context->diagnostics == nullptr)
            return;

        context->diagnostics->deniedCallsTotal += 1;
        context->diagnostics->deniedCallsByApi[api_name] += 1;
        context->diagnostics->lastError = reason;

        Core::Console::Global().Warn("[Plugin] {}", reason);
    }

    BS_PluginResult ValidateCall(HostApiContext *context, PluginApiName api_name)
    {
        if (context == nullptr || context->manifest == nullptr || context->grants == nullptr)
            return BS_PLUGIN_RESULT_INTERNAL_ERROR;

        if (context->services == nullptr || std::this_thread::get_id() != context->services->mainThreadId)
        {
            RegisterDeniedCall(
                context,
                api_name,
                std::format(
                    "Denied API '{}' for plugin '{}' from non-main thread",
                    ToString(api_name),
                    context->manifest->id));
            return BS_PLUGIN_RESULT_INTERNAL_ERROR;
        }

        const bool is_declared = context->manifest->declaredApiSet.contains(api_name);
        if (!is_declared)
        {
            if (!(BS::Plugins::IsDebugUndeclaredApiOverrideSupported() && context->grants->allowUndeclaredApisForDebug))
            {
                RegisterDeniedCall(
                    context,
                    api_name,
                    std::format("Denied undeclared API '{}' for plugin '{}'", ToString(api_name), context->manifest->id));
                return BS_PLUGIN_RESULT_DENIED_UNDECLARED_API;
            }
        }

        const PluginPermission required_permission = RequiredPermissionForApi(api_name);
        if (!BS::Plugins::IsPermissionGranted(*context->grants, required_permission))
        {
            RegisterDeniedCall(
                context,
                api_name,
                std::format(
                    "Denied API '{}' for plugin '{}' (permission '{}' not granted)",
                    ToString(api_name),
                    context->manifest->id,
                    ToString(required_permission)));
            return BS_PLUGIN_RESULT_DENIED_PERMISSION;
        }

        return BS_PLUGIN_RESULT_OK;
    }

    BS_PluginResult RequestEditorContextImpl(void *user_data, BS_EditorContext *out_context)
    {
        HostApiContext *context = reinterpret_cast<HostApiContext *>(user_data);
        const BS_PluginResult precheck = ValidateCall(context, PluginApiName::RequestEditorContext);
        if (precheck != BS_PLUGIN_RESULT_OK)
            return precheck;

        if (out_context == nullptr)
            return BS_PLUGIN_RESULT_INVALID_ARGUMENT;

        std::memset(out_context, 0, sizeof(BS_EditorContext));

        if (context->services == nullptr || context->services->editorStateProvider == nullptr)
            return BS_PLUGIN_RESULT_UNAVAILABLE;

        std::string level_name;
        const bool has_level = context->services->editorStateProvider->GetCurrentLevelName(level_name);
        out_context->hasOpenLevel = has_level ? 1 : 0;

        if (has_level)
            std::snprintf(out_context->levelName, sizeof(out_context->levelName), "%s", level_name.c_str());

        return BS_PLUGIN_RESULT_OK;
    }

    BS_PluginResult RequestLevelEditImpl(void *user_data, int32_t x, int32_t y, BS_TileData tile)
    {
        HostApiContext *context = reinterpret_cast<HostApiContext *>(user_data);
        const BS_PluginResult precheck = ValidateCall(context, PluginApiName::RequestLevelEdit);
        if (precheck != BS_PLUGIN_RESULT_OK)
            return precheck;

        if (context->services == nullptr || context->services->levelEditSink == nullptr)
            return BS_PLUGIN_RESULT_UNAVAILABLE;

        std::string error;
        if (!context->services->levelEditSink->ApplyTileEdit(x, y, tile, error))
        {
            RegisterDeniedCall(
                context,
                PluginApiName::RequestLevelEdit,
                std::format("Level edit failed for plugin '{}': {}", context->manifest->id, error));
            return BS_PLUGIN_RESULT_INTERNAL_ERROR;
        }

        return BS_PLUGIN_RESULT_OK;
    }

    BS_PluginResult HostLogImpl(void *user_data, BS_LogLevel level, const char *message)
    {
        HostApiContext *context = reinterpret_cast<HostApiContext *>(user_data);
        const BS_PluginResult precheck = ValidateCall(context, PluginApiName::HostLog);
        if (precheck != BS_PLUGIN_RESULT_OK)
            return precheck;

        if (message == nullptr)
            return BS_PLUGIN_RESULT_INVALID_ARGUMENT;

        switch (level)
        {
            case BS_LOG_LEVEL_INFO:
                Core::Console::Global().Log("[Plugin:{}] {}", context->manifest->id, message);
                break;
            case BS_LOG_LEVEL_WARN:
                Core::Console::Global().Warn("[Plugin:{}] {}", context->manifest->id, message);
                break;
            case BS_LOG_LEVEL_ERROR:
                Core::Console::Global().Err("[Plugin:{}] {}", context->manifest->id, message);
                break;
            default:
                return BS_PLUGIN_RESULT_INVALID_ARGUMENT;
        }

        return BS_PLUGIN_RESULT_OK;
    }
}

void BS::Plugins::InitializeHostApiTable(BS_HostAPI &out_api, HostApiContext *context)
{
    out_api.abi_version = BS_PLUGIN_ABI_VERSION;
    out_api.user_data = context;
    out_api.RequestEditorContext = RequestEditorContextImpl;
    out_api.RequestLevelEdit = RequestLevelEditImpl;
    out_api.HostLog = HostLogImpl;
}
