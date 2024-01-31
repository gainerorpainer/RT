#pragma once

#include <array>

#include <Shapes.h>

namespace Scene
{
    using namespace Shapes;

    std::array<Shape *, 4> Objects = {
        new Sphere{"Sky", {255, 255, 255}, {0, 0, 0}, 1000},
        new Plane{"Floor", {100, 100, 100}, {0, 0, 0}, {0, 0, 1}},
        new Sphere{"Red Sphere", {255, 0, 0}, {3, -1, 1}, 1},
        new Sphere{"Blue Sphere", {0, 0, 255}, {4, 1, 1.5}, 1.5}};
}
