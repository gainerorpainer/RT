#pragma once

#include "Types.h"

using namespace Types;

namespace Camera
{
    constexpr Vec3d_t Origin = {0, 0, 0};
    constexpr Vec3d_t Pointing = {1, 0, 0};
    constexpr double FOV = Deg2Rad(90); 
}
