#include "Debug.h"

namespace Debug
{
    void _warn(std::string const &message, std::string const &file, int line)
    {
        std::cout << "WARNING: In " << file << ":" << line << " - " << message << std::endl;
    }

    void _crash(std::string const &message, std::string const &file, int line)
    {
        std::cerr << "WARNING: In " << file << ":" << line << " - " << message << std::endl;
        exit(-1);
    }
}