#pragma once

#include <array>

#include <Shapes.h>

namespace Scene
{
    using namespace Shapes;

    std::array<Shape *, 3> Objects = {
        new Plane{"Floor", {255, 255, 255}, {0, 0, -2}, {0, 0, 1}},
        new Sphere{"Red Sphere", {255, 0, 0}, {3, -1, 0}, 1},
        new Sphere{"Blue Sphere", {0, 0, 255}, {4, 1, 0}, 1.5}};
}
