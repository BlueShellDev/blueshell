#pragma once

// file: debug.hpp
// purpose: simple functions for easy runtime debugging

// std
#include <iostream>
#include <source_location>
#include <format>

namespace Debug
{
    static bool ignore_verbose = false;

    enum LogType_ 
    {
        /* default option, just prints message to console */
        LogType_Info, 
        /* prints message with information about where the log was called */
        LogType_Trace, 
        /* use this option if you want to silence at runtime 
        (make sure to set Debug::ignore_verbose to true) */
        LogType_Verbose 
    };

    static void log
    (
        /* what to log */
        const auto message,
        LogType_ type = LogType_Info,
        /* leave this default for most cases */
        const std::source_location& location = std::source_location::current()
    )
    {
        switch (type)
        {
        case LogType_Info:
            /* standard log message */
            std::cout
                << "[BlueShell]: "
                << message                                            
                << std::endl;
            break;

        case LogType_Trace:
            /* use this type if you're gonna log an exception to the console */
            std::cerr 
                << "[TRACE]: "
                << std::format("\nfile: {}\nline: {}\nfunc: {}\nmessage: {}\n",
                    location.file_name(),
                    location.line(),
                    location.function_name(),
                    message);
            break;

        case LogType_Verbose:
            /* use this on logs that repeat i.e in loops (can be silenced) */
            if (ignore_verbose)
                return;
            else 
                std::cout
                << "<BlueShell>: "
                << message
                << std::endl;
            break;

        default:
            /* dev put wrong type */
            std::cout
                << "!invalid log type!\n"
                << message
                << std::endl;
            break;
        }
    }
}
