#pragma once

#include <string>

#define DEBUG_WARN(Message) Debug::_warn(Message, __FILE__, __LINE__)
#define DEBUG_CRASH(Message) Debug::_crash(Message, __FILE__, __LINE__)
#define DEBUG_ASSERT(Condition, Message) Debug::_assert(Condition, Message, __FILE__, __LINE__)
#define DEBUG_BREAKIF(Condition) Debug::_breakif(Condition)

constexpr bool AlmostSame(double a, double b)
{
    return abs(a - b) <= 1e-4;
}

namespace Debug
{
    /// @brief INTERNAL DO NOT USE
    void _warn(std::string const &message, std::string const &file, int line);
    /// @brief INTERNAL DO NOT USE
    void _crash(std::string const &message, std::string const &file, int line);
    /// @brief INTERNAL DO NOT USE
    void _assert(bool condition, std::string const &message, std::string const &file, int line);
    /// @brief INTERNAL DO NOT USE
    void _breakif(bool condition);
}