#pragma once

#include "Shapes.h"
#include "Debug.h"

namespace Tests
{
    inline void RunTests()
    {
        Shapes::Sphere const sphere{"Skysphere",
                                    Materials::Material{},
                                    {0, 0, 0},
                                    1000};
        Primitives::Line const line{
            .Origin = {0, 0, 1}, .Direction = Vec3d{0.588, -0.588, -0.555}.ToNormalized()};

        auto const hitEvent = sphere.CheckHit(line);
        DEBUG_ASSERT(hitEvent.has_value(), "Line should intersect)");
        DEBUG_ASSERT(hitEvent->DistanceToSurface < 1000, "Intersection should be closest");
    }
}