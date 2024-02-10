#pragma once

#include <optional>
#include <cassert>
#include <string>

#include "Primitives.h"

namespace Shapes
{
    using namespace Primitives;

    struct HitEvent
    {
        double DistanceToSurface;
        Vec3d SurfaceNormal;
        Line ReflectedRay;
    };

    struct MaterialInfo
    {
        bool IsLightsource;
        ColorD_t Emission;
        ColorD_t ColorFilter;
        double DiffusionFactor;

        static MaterialInfo MakeAbsorbing(Color_t const &visibleColor, double diffusionFactor = 0.0);

        static MaterialInfo MakeEmitting(Color_t const &emission);
    };

    class Shape
    {
    public:
        std::string const Label;
        MaterialInfo const Material;

        virtual std::optional<HitEvent> CheckHit(Line const &line) const = 0;

    protected:
        Shape(std::string const &label, MaterialInfo const &material);

        static Vec3d Reflect(Vec3d const &direction, Vec3d const &normal);
    };

    struct Sphere : public Shape
    {
        Vec3d Centerpoint;
        double Radius;

        Sphere(std::string const &label, MaterialInfo const &material, Vec3d center, double radius);

        std::optional<HitEvent> CheckHit(Line const &line) const override;
    };

    struct Plane : public Shape
    {
        Vec3d Pin;
        Vec3d Normal;

        Plane(std::string const &label, MaterialInfo const &material, Vec3d pin, Vec3d planeNormal);

        std::optional<HitEvent> CheckHit(Line const &line) const;
    };
}