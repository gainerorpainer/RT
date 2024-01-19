#pragma once

#include <array>
#include <tuple>
#include <math.h>

namespace Types
{
    using Color_t = std::array<unsigned char, 3>;
    using Vec3d_t = std::array<double, 3>;

    using Line_t = std::tuple<Vec3d_t, Vec3d_t>;

    constexpr double Deg2Rad(double deg)
    {
        return deg * M_PI / 180.0;
    }

    constexpr double Rad2Deg(double deg)
    {
        return deg * 180.0 / M_PI;
    }
}
