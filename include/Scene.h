#pragma once

#include <array>

#include "Types.h"
#include <Shapes.h>

namespace Scene
{
    using namespace Types;
    using namespace Shapes;

    std::array<Shape *, 2> Objects = {
        new Sphere{{255, 0, 0}, {2, -1, 0}, 1},
        new Sphere{{0, 0, 255}, {2, 1, 0}, 1}
    };
}
