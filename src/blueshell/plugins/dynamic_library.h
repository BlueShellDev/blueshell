#pragma once

#include <filesystem>
#include <string>

namespace BS::Plugins
{
    class DynamicLibrary
    {
    public:
        DynamicLibrary() = default;
        ~DynamicLibrary();

        DynamicLibrary(const DynamicLibrary &) = delete;
        DynamicLibrary &operator=(const DynamicLibrary &) = delete;
        DynamicLibrary(DynamicLibrary &&other) noexcept;
        DynamicLibrary &operator=(DynamicLibrary &&other) noexcept;

        bool Open(const std::filesystem::path &path, std::string *out_error);
        void *ResolveSymbol(const char *symbol_name, std::string *out_error) const;
        void Close();

        bool IsOpen() const;

    private:
#ifdef _WIN32
        void *handle_{nullptr};
#else
        void *handle_{nullptr};
#endif
    };
};
