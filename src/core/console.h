#pragma once
#include <iostream>
#include <format>

namespace Core
{
    class Console
    {
        Console() = default;
        ~Console() = default;

    public:
        std::string buffer;

        Console(const Console &) = delete;
        Console(Console &&) = delete;
        Console &operator=(const Console &) = delete;
        Console &operator=(Console &&) = delete;

        static Console &Global()
        {
            static Console instance;
            return instance;
        }
        template <class... _Args>
        void Log(const std::format_string<_Args...> &fmt, _Args... args)
        {
            auto message = "[INFO] " + std::vformat(fmt.get(), std::make_format_args(args...)) + "\n";
            buffer += message;
            std::cout << message;
        };
        template <class... _Args>
        void Warn(const std::format_string<_Args...> &fmt, _Args... args)
        {
            auto message = "[WARN] " + std::vformat(fmt.get(), std::make_format_args(args...)) + "\n";
            buffer += message;
            std::cout << message;
        };
        template <class... _Args>
        void Err(const std::format_string<_Args...> &fmt, _Args... args)
        {
            auto message = "[ERR] " + std::vformat(fmt.get(), std::make_format_args(args...)) + "\n";
            buffer += message;
            std::cerr << message;
        };
    };
}