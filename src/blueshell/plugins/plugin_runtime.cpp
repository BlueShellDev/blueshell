#include "plugin_runtime.h"

#include <exception>
#include <format>

bool BS::Plugins::PluginRuntime::Load(
    const PluginManifest *manifest,
    const PluginGrants *grants,
    HostApiServices *services,
    PluginDiagnostics *diagnostics,
    std::string *out_error)
{
    Unload();

    if (manifest == nullptr || grants == nullptr || services == nullptr || diagnostics == nullptr)
    {
        if (out_error != nullptr)
            *out_error = "PluginRuntime::Load called with null dependency";
        return false;
    }

    if (!dynamicLibrary_.Open(manifest->entryAbsolutePath, out_error))
        return false;

    if (!ResolveExports(out_error))
    {
        Unload();
        return false;
    }

    const uint32_t abi_version = getAbiVersion_();
    if (abi_version != BS_PLUGIN_ABI_VERSION)
    {
        if (out_error != nullptr)
        {
            *out_error = std::format(
                "Plugin ABI mismatch for '{}': plugin={} host={}",
                manifest->id,
                abi_version,
                BS_PLUGIN_ABI_VERSION);
        }

        Unload();
        return false;
    }

    hostContext_.manifest = manifest;
    hostContext_.grants = grants;
    hostContext_.diagnostics = diagnostics;
    hostContext_.services = services;

    InitializeHostApiTable(hostApi_, &hostContext_);

    try
    {
        if (!init_(&hostApi_, &pluginState_))
        {
            if (out_error != nullptr)
                *out_error = std::format("Plugin '{}' rejected initialization", manifest->id);

            Unload();
            return false;
        }
    }
    catch (const std::exception &ex)
    {
        if (out_error != nullptr)
            *out_error = std::format("Plugin '{}' init threw exception: {}", manifest->id, ex.what());

        Unload();
        return false;
    }
    catch (...)
    {
        if (out_error != nullptr)
            *out_error = std::format("Plugin '{}' init threw unknown exception", manifest->id);

        Unload();
        return false;
    }

    return true;
}

void BS::Plugins::PluginRuntime::Unload()
{
    if (dynamicLibrary_.IsOpen() && shutdown_ != nullptr)
    {
        try
        {
            shutdown_(pluginState_);
        }
        catch (...)
        {
            // Prevent plugin faults from crashing host during unload.
        }
    }

    pluginState_ = nullptr;
    getAbiVersion_ = nullptr;
    init_ = nullptr;
    tick_ = nullptr;
    shutdown_ = nullptr;

    hostContext_ = {};
    hostApi_ = {};

    dynamicLibrary_.Close();
}

bool BS::Plugins::PluginRuntime::Tick(double delta_seconds, std::string *out_error)
{
    if (!IsLoaded() || tick_ == nullptr)
        return false;

    try
    {
        tick_(pluginState_, delta_seconds);
        return true;
    }
    catch (const std::exception &ex)
    {
        if (out_error != nullptr)
            *out_error = std::format("Plugin '{}' tick exception: {}", hostContext_.manifest->id, ex.what());
        return false;
    }
    catch (...)
    {
        if (out_error != nullptr)
            *out_error = std::format("Plugin '{}' tick unknown exception", hostContext_.manifest->id);
        return false;
    }
}

bool BS::Plugins::PluginRuntime::IsLoaded() const
{
    return dynamicLibrary_.IsOpen() && init_ != nullptr && tick_ != nullptr && shutdown_ != nullptr;
}

bool BS::Plugins::PluginRuntime::ResolveExports(std::string *out_error)
{
    getAbiVersion_ = reinterpret_cast<GetABIVersionFn>(dynamicLibrary_.ResolveSymbol("BS_Plugin_GetABIVersion", out_error));
    if (getAbiVersion_ == nullptr)
        return false;

    init_ = reinterpret_cast<InitFn>(dynamicLibrary_.ResolveSymbol("BS_Plugin_Init", out_error));
    if (init_ == nullptr)
        return false;

    tick_ = reinterpret_cast<TickFn>(dynamicLibrary_.ResolveSymbol("BS_Plugin_Tick", out_error));
    if (tick_ == nullptr)
        return false;

    shutdown_ = reinterpret_cast<ShutdownFn>(dynamicLibrary_.ResolveSymbol("BS_Plugin_Shutdown", out_error));
    if (shutdown_ == nullptr)
        return false;

    return true;
}
