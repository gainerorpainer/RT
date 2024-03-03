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
        FloatingType_t DistanceToSurface;
        Vec3d SurfaceNormal;
        Line ReflectedRay;
    };

    /// @brief VIRTUAL
    class Shape
    {
    public:
        std::string Label;

        virtual std::optional<HitEvent> CheckHit(Line const &line) const = 0;
        virtual Materials::Material const &GetMaterial(HitEvent const & hitEvent) const = 0;

    protected:
        Shape(std::string const &label);
    };

    struct Sphere : public Shape
    {
        Vec3d Centerpoint;
        FloatingType_t Radius;

        Sphere(std::string const &label, Materials::Material const &material, Vec3d center, FloatingType_t radius);

        std::optional<HitEvent> CheckHit(Line const &line) const override;
        Materials::Material const &GetMaterial(HitEvent const & hitEvent) const override;

    private:
        Materials::Material Material;
    };

    struct Plane : public Shape
    {
        Vec3d Pin;
        Vec3d Normal;

        Plane(std::string const &label, Materials::Material const &material, Vec3d pin, Vec3d planeNormal);

        std::optional<HitEvent> CheckHit(Line const &line) const override;
        Materials::Material const &GetMaterial(HitEvent const & hitEvent) const override;

    protected:
        Plane(std::string const &label, Vec3d pin, Vec3d planeNormal);

    private:
        Materials::Material Material;
    };

    struct CheckerboardPlane : public Plane
    {
        CheckerboardPlane(std::string const &label, std::pair<Materials::Material, Materials::Material> materials, FloatingType_t width, Vec3d pin, Vec3d normal);

        Materials::Material const &GetMaterial(HitEvent const & hitEvent) const override;
    private:
        std::pair<Materials::Material, Materials::Material> Materials;
        FloatingType_t Width;
    };
}