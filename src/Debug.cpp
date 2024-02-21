#include "Debug.h"

#include <iostream>
#include <map>
#include <csignal>

namespace Debug
{
    static std::map<std::size_t, unsigned int> Counter;

    inline unsigned int IncrementCounter(std::string const &file, unsigned int line)
    {
        auto const id = std::hash<std::string>{}(file) + std::hash<unsigned int>{}(line);
        if (!Counter.contains(id))
            return Counter[id] = 1;
        return ++Counter[id];
    }

    inline bool isPowerOfTwo(unsigned int n)
    {
        return (n & (~(n - 1))) == n;
    }

#ifdef NDEBUG
    void _print(std::string const &message)
    {
    }
    void _warn(std::string const &message, std::string const &file, int line)
    {
    }
    void _crash(std::string const &message, std::string const &file, int line)
    {
    }
    void _assert(bool condition, std::string const &message, std::string const &file, int line)
    {
    }
#else
    void _print(std::string const &message)
    {
        std::cout << message << "\n";
    }
    
    void _warn(std::string const &message, std::string const &file, int line)
    {
        unsigned int const occurenceCounter = IncrementCounter(file, line);

        // Show only every 2nd, 4th, 8th, 16th occurence etc
        if (occurenceCounter > 2 && !isPowerOfTwo(occurenceCounter))
            return;

        std::cout << "WARNING: In " << file << ":" << line << " (#" << occurenceCounter << ") - " << message << std::endl;
    }
    void _crash(std::string const &message, std::string const &file, int line)
    {
        // try to alert debugger
        __debugbreak;
        std::raise(SIGABRT);

        std::cerr << "ERROR: In " << file << ":" << line << " - " << message << std::endl;
        exit(-1);
    }
    void _assert(bool condition, std::string const &message, std::string const &file, int line)
    {
        if (condition)
            return;

        _crash(message, file, line);
    }
#endif
}