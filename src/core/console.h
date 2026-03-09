#pragma once
#include <iostream>
#include <vector>
#include <format>

namespace Core
{
    struct ConsoleMsg
    {
        enum class Type
        {
            Log, Warning, Error
        } type{Type::Log};

        std::string text;

        inline ConsoleMsg(Type t, const std::string& msg) : type(t) 
        {
            switch (t)
            {
                case Type::Log:
                    text = " [*] " + msg + '\n';
                    break;
                case Type::Warning:
                    text = " [!] " + msg + '\n';
                    break;
                case Type::Error:
                    text = " [X] " + msg + '\n';
                    break;
                default:
                    break;
            }
        };
    };

    class Console
    {
        Console() = default;
        ~Console() = default;

    public:
        std::vector<ConsoleMsg> buffer;

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
            ConsoleMsg message(ConsoleMsg::Type::Log, std::vformat(fmt.get(), std::make_format_args(args...)));
            buffer.emplace_back(message);
            std::cout << "\033[0m" << message.text;
        };

        template <class... _Args>
        void Warn(const std::format_string<_Args...> &fmt, _Args... args)
        {
            ConsoleMsg message(ConsoleMsg::Type::Warning, std::vformat(fmt.get(), std::make_format_args(args...)));
            buffer.emplace_back(message);
            std::cout << "\e[0;33m" << message.text;
        };

        template <class... _Args>
        void Err(const std::format_string<_Args...> &fmt, _Args... args)
        {
            ConsoleMsg message(ConsoleMsg::Type::Error, std::vformat(fmt.get(), std::make_format_args(args...)));
            buffer.emplace_back(message);
            std::cout << "\033[31m" << message.text;
        };
    };
}