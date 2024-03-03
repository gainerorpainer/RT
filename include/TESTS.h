#pragma once

#include "Shapes.h"
#include "Debug.h"

namespace Tests
{
    inline void _test_sphere_reflection()
    {
        Shapes::Sphere const sphere{"Skysphere",
                                    Materials::Material{},
                                    {0, 0, 0},
                                    1000};
        Primitives::Line const line{
            .Origin = {854.804, -87.841, 0}, .Direction = Vec3d{-0.3057, 0.9462, -0.1056}.ToNormalized()};

        auto const hitEvent = sphere.CheckHit(line);

        DEBUG_ASSERT(hitEvent.has_value(), "Line should intersect)");
        DEBUG_ASSERT(hitEvent->DistanceToSurface < 1000, "Intersection should be closest");

        FloatingType_t const isParallel = abs((hitEvent->ReflectedRay.Origin - sphere.Centerpoint) * hitEvent->ReflectedRay.Direction);
        DEBUG_ASSERT(isParallel > 0.9, "Reflected ray should point inwards!");
    }

    inline void _test_probes()
    {
        constexpr unsigned int RAYCOUNT = 5;
        auto const RandFloat = []()
        { return (std::rand() % 2) ? 0.5 : 1; };

        const Vec3d origin = {0, 0, 0};
        const Vec3d surfaceNormal = {0, 0, 1};
        const Vec3d reflection = Vec3d{1, 0, 1}.ToNormalized();

        std::array<Line, RAYCOUNT> rayProbes = {};

        // Start from surface normals
        Line probingRay{origin, surfaceNormal};

        // spawn random rays
        for (size_t j = 0; j < RAYCOUNT; j++)
        {
            // rotate away from surface normal in the plane (surface normal) x (reflection)
            probingRay.Direction = probingRay.Direction.RotateAboutPlane(surfaceNormal, reflection, RandFloat() * Deg2Rad(60));

            // start rotating about the normal in appropriate steps
            probingRay.Direction = probingRay.Direction.RotateAboutAxis(surfaceNormal, 0.8 * RandFloat() * Deg2Rad(360));

            DEBUG_ASSERT(AlmostSame(probingRay.Direction.GetNorm(), 1.0), "Rotation is bad for vector");

            DEBUG_ASSERT(probingRay.Direction.Z > 0, "Probe penetrates surface");
        }
    }

    inline void RunTests()
    {
        _test_probes();
    }
}