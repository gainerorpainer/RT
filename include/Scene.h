#pragma once

#include <array>

#include "Shapes.h"

namespace Scene
{
    using namespace Shapes;

    std::array<Shape const *, 5> const Objects = {
        new Plane{"Sunplane",
                  Materials::Material{}.MakeEmissive({255, 255, 255}),
                  {-100, 100, 100},
                  {2, -1, -1}},
        new Sphere{"Skysphere",
                   Materials::Material{}.MakeAbsorbing({50, 100, 255}),
                   {0, 0, 0},
                   1000},
        new Plane{"Floor",
                  Materials::Material{}.MakeAbsorbing({255, 255, 255}).MakeDiffuse(0.8),
                  {0, 0, 0},
                  {0, 0, 1}},
        new Sphere{"Red Sphere",
                   Materials::Material{}.MakeAbsorbing({230, 100, 100}).MakeDiffuse(0.5),
                   {3, -1, 1},
                   1},
        new Sphere{"Blue Sphere",
                   Materials::Material{}.MakeAbsorbing({25, 25, 230}).MakeTotallyReflecting(Deg2Rad(15)),
                   {4, 1, 1.5},
                   1.5}};
}
