#pragma once

#include <string>
#include <iostream>

#define DEBUG_WARN(Message) Debug::_warn(Message, __FILE__, __LINE__)

#define DEBUG_CRASH(Message) Debug::_crash(Message, __FILE__, __LINE__)

namespace Debug
{
    /// @brief INTERNAL, DO NOT USE: Display a warning
    /// @param message Message to show
    /// @param file In which file
    /// @param line On which line
    void _warn(std::string const & message, std::string const & file, int line);

    void _crash(std::string const & message, std::string const & file, int line);
} 
