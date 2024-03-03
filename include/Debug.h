#pragma once

#include <string>
#include <iostream>

#define DEBUG_PRINT(Message) Debug::_print(Message)
#define DEBUG_WARN(Message) Debug::_warn(Message, __FILE__, __LINE__)
#define DEBUG_CRASH(Message) Debug::_crash(Message, __FILE__, __LINE__)
#define DEBUG_ASSERT(Condition, Message) Debug::_assert(Condition, Message, __FILE__, __LINE__)

template <typename T1, typename T2>
constexpr bool AlmostSame(T1 a, T2 b)
{
    return abs(a - b) <= 1e-2;
}

namespace Debug
{
    /// @brief INTERNAL DO NOT USE
    void _print(std::string const & message);
    /// @brief INTERNAL DO NOT USE
    void _warn(std::string const &message, std::string const &file, int line);
    /// @brief INTERNAL DO NOT USE
    void _crash(std::string const &message, std::string const &file, int line);
    /// @brief INTERNAL DO NOT USE
    void _assert(bool condition, std::string const &message, std::string const &file, int line);
}