#pragma once

#include <optional>
#include <string>

#include "Primitives.h"
#include "Materials.h"

namespace Shapes
{
    using namespace Primitives;

    struct HitEvent
    {
        double DistanceToSurface;
        Vec3d SurfaceNormal;
        Line ReflectedRay;
    };

    class Shape
    {
    public:
        std::string Label;
        Materials::Material Material;

        virtual std::optional<HitEvent> CheckHit(Line const &line) const noexcept = 0;

    protected:
        Shape(std::string const &label, Materials::Material const &material);

        static Vec3d Reflect(Vec3d const &direction, Vec3d const &normal);
    };

    struct Sphere : public Shape
    {
        Vec3d Centerpoint;
        double Radius;

        Sphere(std::string const &label, Materials::Material const &material, Vec3d center, double radius);

        std::optional<HitEvent> CheckHit(Line const &line) const noexcept override;
    };

    struct Plane : public Shape
    {
        Vec3d Pin;
        Vec3d Normal;

        Plane(std::string const &label, Materials::Material const &material, Vec3d pin, Vec3d planeNormal);

        std::optional<HitEvent> CheckHit(Line const &line) const noexcept;
    };
}