#pragma once

#include <string>
#include <iostream>

#define DEBUG_WARN(Message) Debug::_warn(Message, __FILE__, __LINE__)

#define DEBUG_CRASH(Message) Debug::_crash(Message, __FILE__, __LINE__)

#define DEBUG_ASSERT(Condition, Message) Debug::_assert(Condition, Message, __FILE__, __LINE__)

constexpr bool AlmostSame(double a, double b)
{
    return abs(a - b) <= 1e-10;
}

namespace Debug
{
#ifdef NDEBUG
    inline void _warn(std::string const &message, std::string const &file, int line)
    {
    }
    inline void _crash(std::string const &message, std::string const &file, int line)
    {
    }
    inline void _assert(bool condition, std::string const &message, std::string const &file, int line)
    {
    }
#else
    /// @brief INTERNAL DO NOT USE
    inline void _warn(std::string const &message, std::string const &file, int line)
    {
        std::cout << "WARNING: In " << file << ":" << line << " - " << message << std::endl;
    }
    /// @brief INTERNAL DO NOT USE
    inline void _crash(std::string const &message, std::string const &file, int line)
    {
        std::cerr << "ERROR: In " << file << ":" << line << " - " << message << std::endl;
        exit(-1);
    }
    /// @brief INTERNAL DO NOT USE
    inline void _assert(bool condition, std::string const &message, std::string const &file, int line)
    {
        if (condition)
            return;

        _crash(message, file, line);
    }
#endif
}
