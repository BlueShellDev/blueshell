#include <cstdio>
#include <cstring>

#include "blueshell/plugins/plugin_api.h"

namespace
{
    struct SamplePluginState
    {
        const BS_HostAPI *host{};
        double elapsedSeconds{0.0};
    };

    void HostLogf(const BS_HostAPI *host, BS_LogLevel level, const char *fmt, const char *arg = nullptr)
    {
        if (host == nullptr || host->HostLog == nullptr || fmt == nullptr)
            return;

        char buffer[512] = {};
        if (arg != nullptr)
            std::snprintf(buffer, sizeof(buffer), fmt, arg);
        else
            std::snprintf(buffer, sizeof(buffer), "%s", fmt);

        host->HostLog(host->user_data, level, buffer);
    }

    const char *ResultToString(BS_PluginResult result)
    {
        switch (result)
        {
            case BS_PLUGIN_RESULT_OK:
                return "OK";
            case BS_PLUGIN_RESULT_DENIED_UNDECLARED_API:
                return "DENIED_UNDECLARED_API";
            case BS_PLUGIN_RESULT_DENIED_PERMISSION:
                return "DENIED_PERMISSION";
            case BS_PLUGIN_RESULT_UNAVAILABLE:
                return "UNAVAILABLE";
            case BS_PLUGIN_RESULT_INVALID_ARGUMENT:
                return "INVALID_ARGUMENT";
            case BS_PLUGIN_RESULT_INTERNAL_ERROR:
                return "INTERNAL_ERROR";
            default:
                return "UNKNOWN";
        }
    }
}

extern "C" BS_PLUGIN_EXPORT uint32_t BS_Plugin_GetABIVersion()
{
    return BS_PLUGIN_ABI_VERSION;
}

extern "C" BS_PLUGIN_EXPORT uint8_t BS_Plugin_Init(const BS_HostAPI *host_api, void **plugin_state)
{
    if (host_api == nullptr || plugin_state == nullptr)
        return 0;

    if (host_api->abi_version != BS_PLUGIN_ABI_VERSION)
        return 0;

    auto *state = new SamplePluginState();
    state->host = host_api;

    *plugin_state = state;

    HostLogf(host_api, BS_LOG_LEVEL_INFO, "Sample Presence Plugin initialized.");
    return 1;
}

extern "C" BS_PLUGIN_EXPORT void BS_Plugin_Tick(void *plugin_state, double delta_seconds)
{
    auto *state = reinterpret_cast<SamplePluginState *>(plugin_state);
    if (state == nullptr || state->host == nullptr)
        return;

    state->elapsedSeconds += delta_seconds;
    if (state->elapsedSeconds < 3.0)
        return;

    state->elapsedSeconds = 0.0;

    BS_EditorContext context = {};
    const BS_PluginResult result = state->host->RequestEditorContext(state->host->user_data, &context);

    if (result == BS_PLUGIN_RESULT_OK)
    {
        if (context.hasOpenLevel)
            HostLogf(state->host, BS_LOG_LEVEL_INFO, "[SamplePresence] Current level: %s", context.levelName);
        else
            HostLogf(state->host, BS_LOG_LEVEL_INFO, "[SamplePresence] No level is currently open.");
        return;
    }

    char buffer[256] = {};
    std::snprintf(buffer, sizeof(buffer), "[SamplePresence] RequestEditorContext failed: %s", ResultToString(result));
    HostLogf(state->host, BS_LOG_LEVEL_WARN, buffer);
}

extern "C" BS_PLUGIN_EXPORT void BS_Plugin_Shutdown(void *plugin_state)
{
    auto *state = reinterpret_cast<SamplePluginState *>(plugin_state);
    if (state != nullptr && state->host != nullptr)
        HostLogf(state->host, BS_LOG_LEVEL_INFO, "Sample Presence Plugin shutting down.");

    delete state;
}
