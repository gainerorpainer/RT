#pragma once

#include <array>
#include <cmath>

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
                   Materials::Material{}.MakeEmissive({100, 120, 255}),
                   {0, 0, 0},
                   1000},
        new CheckerboardPlane{"Floor",
                              std::make_pair(Materials::Material{}.MakeAbsorbing({255, 255, 255}).MakeDiffuse(1), Materials::Material{}.MakeAbsorbing({0, 0, 0})),
                              (FloatingType_t)1,
                              {0, 0, 0},
                              {0, 0, 1}},
        new Sphere{"Red Sphere", Materials::Material{}.MakeAbsorbing({230, 100, 100}).MakeDiffuse(0.5).MakeTotallyReflecting(Deg2Rad(80)), {3, -1, 1}, 1},
        new Sphere{"Blue Sphere", Materials::Material{}.MakeAbsorbing({150, 150, 255}).MakeDiffuse(0.1).MakeTotallyReflecting(Deg2Rad(60)), {4, 1, 1.5}, 1.5}};
}
