#pragma once

#include <array>

#include <Shapes.h>

namespace Scene
{
    using namespace Shapes;

    std::array<Shape *, 4> Objects = {
        new Sphere{"Sky", MaterialInfo::MakeEmitting({255, 255, 255}), {0, 0, 0}, 1000},
        new Plane{"Floor", MaterialInfo::MakeAbsorbing({100, 100, 100}), {0, 0, 0}, {0, 0, 1}},
        new Sphere{"Red Sphere", MaterialInfo::MakeAbsorbing({230, 25, 25}), {3, -1, 1}, 1},
        new Sphere{"Blue Sphere", MaterialInfo::MakeAbsorbing({25, 25, 230}), {4, 1, 1.5}, 1.5}};
}
