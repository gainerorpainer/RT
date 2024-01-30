#pragma once

#include <array>

#include <Shapes.h>

namespace Scene
{
    using namespace Shapes;

    std::array<Shape *, 2> Objects = {
        new Sphere{{255, 0, 0}, {3, -1, 0}, 1},
        new Sphere{{0, 0, 255}, {4, 1, 0}, 1.5}};
}
