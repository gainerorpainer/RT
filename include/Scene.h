#pragma once

#include <vector>

#include "Shapes.h"

namespace Scene
{
    using namespace Shapes;

    std::vector<Shape *> const Objects = {
        new Plane{"Sunplane", MaterialInfo::MakeEmitting({255, 255, 255}), {-100, 100, 100}, {2, -1, -1}},
        new Sphere{"Skysphere", MaterialInfo::MakeEmitting({50, 100, 255}), {0, 0, 0}, 1000},
        new Plane{"Floor", MaterialInfo::MakeAbsorbing({200, 200, 200}, 0.8), {0, 0, 0}, {0, 0, 1}},
        new Sphere{"Red Sphere", MaterialInfo::MakeAbsorbing({230, 100, 100}), {3, -1, 1}, 1},
        new Sphere{"Blue Sphere", MaterialInfo::MakeAbsorbing({25, 25, 230}), {4, 1, 1.5}, 1.5}};
}
