#pragma once

#include <string>

#include "dynamic_library.h"
#include "plugin_host_api.h"

namespace BS::Plugins
{
    class PluginRuntime
    {
    public:
        bool Load(
            const PluginManifest *manifest,
            const PluginGrants *grants,
            HostApiServices *services,
            PluginDiagnostics *diagnostics,
            std::string *out_error);

        void Unload();
        bool Tick(double delta_seconds, std::string *out_error);

        bool IsLoaded() const;

    private:
        using GetABIVersionFn = uint32_t (*)();
        using InitFn = uint8_t (*)(const BS_HostAPI *, void **);
        using TickFn = void (*)(void *, double);
        using ShutdownFn = void (*)(void *);

        bool ResolveExports(std::string *out_error);

        DynamicLibrary dynamicLibrary_;
        HostApiContext hostContext_;
        BS_HostAPI hostApi_{};

        GetABIVersionFn getAbiVersion_{};
        InitFn init_{};
        TickFn tick_{};
        ShutdownFn shutdown_{};

        void *pluginState_{nullptr};
    };
};
