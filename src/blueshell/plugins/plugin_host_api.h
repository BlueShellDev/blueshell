#pragma once

#include <string>
#include <thread>
#include <unordered_map>

#include "core/console.h"
#include "plugin_api.h"
#include "plugin_grants.h"

namespace BS::Plugins
{
    struct EditorStateProvider
    {
        virtual ~EditorStateProvider() = default;
        virtual bool GetCurrentLevelName(std::string &out_level_name) = 0;
    };

    struct LevelEditSink
    {
        virtual ~LevelEditSink() = default;
        virtual bool ApplyTileEdit(int32_t x, int32_t y, const BS_TileData &tile, std::string &out_error) = 0;
    };

    struct HostApiServices
    {
        EditorStateProvider *editorStateProvider{nullptr};
        LevelEditSink *levelEditSink{nullptr};
        std::thread::id mainThreadId{};
    };

    struct PluginDiagnostics
    {
        std::string lastError;
        std::unordered_map<PluginApiName, uint32_t, EnumClassHash> deniedCallsByApi;
        uint32_t deniedCallsTotal{0};
    };

    struct HostApiContext
    {
        const PluginManifest *manifest{nullptr};
        const PluginGrants *grants{nullptr};
        PluginDiagnostics *diagnostics{nullptr};
        HostApiServices *services{nullptr};
    };

    void InitializeHostApiTable(BS_HostAPI &out_api, HostApiContext *context);
};
