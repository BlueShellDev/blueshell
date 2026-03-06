#include "dynamic_library.h"

#include <format>

#if defined(_WIN32)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

BS::Plugins::DynamicLibrary::~DynamicLibrary()
{
    Close();
}

BS::Plugins::DynamicLibrary::DynamicLibrary(DynamicLibrary &&other) noexcept
{
    handle_ = other.handle_;
    other.handle_ = nullptr;
}

BS::Plugins::DynamicLibrary &BS::Plugins::DynamicLibrary::operator=(DynamicLibrary &&other) noexcept
{
    if (this == &other)
        return *this;

    Close();
    handle_ = other.handle_;
    other.handle_ = nullptr;
    return *this;
}

bool BS::Plugins::DynamicLibrary::Open(const std::filesystem::path &path, std::string *out_error)
{
    Close();

#if defined(_WIN32)
    handle_ = reinterpret_cast<void *>(LoadLibraryA(path.string().c_str()));
    if (handle_ == nullptr)
    {
        if (out_error != nullptr)
            *out_error = std::format("LoadLibrary failed for '{}'", path.string());
        return false;
    }
#else
    handle_ = dlopen(path.string().c_str(), RTLD_NOW);
    if (handle_ == nullptr)
    {
        if (out_error != nullptr)
            *out_error = std::format("dlopen failed for '{}': {}", path.string(), dlerror());
        return false;
    }
#endif

    return true;
}

void *BS::Plugins::DynamicLibrary::ResolveSymbol(const char *symbol_name, std::string *out_error) const
{
    if (handle_ == nullptr)
    {
        if (out_error != nullptr)
            *out_error = "ResolveSymbol called on unopened library";
        return nullptr;
    }

#if defined(_WIN32)
    void *symbol = reinterpret_cast<void *>(GetProcAddress(reinterpret_cast<HMODULE>(handle_), symbol_name));
    if (symbol == nullptr && out_error != nullptr)
        *out_error = std::format("GetProcAddress failed for symbol '{}'", symbol_name);
    return symbol;
#else
    dlerror();
    void *symbol = dlsym(handle_, symbol_name);
    const char *error = dlerror();
    if (error != nullptr)
    {
        if (out_error != nullptr)
            *out_error = std::format("dlsym failed for symbol '{}': {}", symbol_name, error);
        return nullptr;
    }
    return symbol;
#endif
}

void BS::Plugins::DynamicLibrary::Close()
{
    if (handle_ == nullptr)
        return;

#if defined(_WIN32)
    FreeLibrary(reinterpret_cast<HMODULE>(handle_));
#else
    dlclose(handle_);
#endif

    handle_ = nullptr;
}

bool BS::Plugins::DynamicLibrary::IsOpen() const
{
    return handle_ != nullptr;
}
