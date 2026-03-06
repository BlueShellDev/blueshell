#pragma once

#include <stdint.h>

#ifdef _WIN32
#define BS_PLUGIN_EXPORT __declspec(dllexport)
#else
#define BS_PLUGIN_EXPORT __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define BS_PLUGIN_ABI_VERSION 1u

typedef enum BS_PluginResult
{
    BS_PLUGIN_RESULT_OK = 0,
    BS_PLUGIN_RESULT_DENIED_UNDECLARED_API = 1,
    BS_PLUGIN_RESULT_DENIED_PERMISSION = 2,
    BS_PLUGIN_RESULT_UNAVAILABLE = 3,
    BS_PLUGIN_RESULT_INVALID_ARGUMENT = 4,
    BS_PLUGIN_RESULT_INTERNAL_ERROR = 5
} BS_PluginResult;

typedef enum BS_LogLevel
{
    BS_LOG_LEVEL_INFO = 0,
    BS_LOG_LEVEL_WARN = 1,
    BS_LOG_LEVEL_ERROR = 2
} BS_LogLevel;

typedef struct BS_EditorContext
{
    uint8_t hasOpenLevel;
    char levelName[256];
} BS_EditorContext;

typedef struct BS_TileData
{
    uint32_t tileId;
    uint16_t behaviorId;
    uint8_t paletteId;
    uint8_t reserved;
} BS_TileData;

typedef BS_PluginResult (*BS_RequestEditorContextFn)(void *user_data, BS_EditorContext *out_context);
typedef BS_PluginResult (*BS_RequestLevelEditFn)(void *user_data, int32_t x, int32_t y, BS_TileData tile);
typedef BS_PluginResult (*BS_HostLogFn)(void *user_data, BS_LogLevel level, const char *message);

typedef struct BS_HostAPI
{
    uint32_t abi_version;
    void *user_data;

    BS_RequestEditorContextFn RequestEditorContext;
    BS_RequestLevelEditFn RequestLevelEdit;
    BS_HostLogFn HostLog;
} BS_HostAPI;

// Required plugin exports.
BS_PLUGIN_EXPORT uint32_t BS_Plugin_GetABIVersion();
BS_PLUGIN_EXPORT uint8_t BS_Plugin_Init(const BS_HostAPI *host_api, void **plugin_state);
BS_PLUGIN_EXPORT void BS_Plugin_Tick(void *plugin_state, double delta_seconds);
BS_PLUGIN_EXPORT void BS_Plugin_Shutdown(void *plugin_state);

#ifdef __cplusplus
}
#endif
