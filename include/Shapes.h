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
        double Distance;
        Line Reflection;
    };

    bool AlmostSame(double a, double b)
    {
        return abs(a - b) <= 1e-10;
    }

    struct MaterialInfo
    {
        bool IsLightsource;
        Color_t Emission;
        ColorD_t TransferFunction;

        static MaterialInfo MakeAbsorbing(Color_t const &visibleColor)
        {
            ColorD_t const filter = {(double)visibleColor[0] / 255.0, (double)visibleColor[1] / 255.0, (double)visibleColor[2] / 255.0};
            return MaterialInfo{.IsLightsource = false, .Emission = {}, .TransferFunction = filter};
        }

        static MaterialInfo MakeEmitting(Color_t const &emission)
        {
            return MaterialInfo{.IsLightsource = true, .Emission = emission, .TransferFunction = {}};
        }
    };

    class Shape
    {
    public:
        std::string const Label;
        MaterialInfo const Material;

        virtual std::optional<HitEvent> CheckHit(Line const &line) const = 0;

    protected:
        Shape(std::string const &label, MaterialInfo const &material) : Label{label}, Material{material}
        {
        }

        static Vec3d Reflect(Vec3d const &direction, Vec3d const &normal)
        {
            // https://math.stackexchange.com/questions/13261/how-to-get-a-reflection-vector
            Vec3d const reflectionDirection = direction - 2.0 * (direction * normal) * normal;
            assert(AlmostSame(reflectionDirection.GetNorm(), 1.0));

            return reflectionDirection;
        }
    };

    struct Sphere : public Shape
    {
        Vec3d const Centerpoint;
        double const Radius;

        Sphere(std::string const &label, MaterialInfo const &material, Vec3d center, double radius)
            : Shape(label, material), Centerpoint{center}, Radius{radius}
        {
        }

        std::optional<HitEvent> CheckHit(Line const &line) const override
        {
            assert(AlmostSame(line.Direction.GetNorm(), 1.0));

            // https://gamedev.stackexchange.com/a/96487

            Vec3d const connectionLine = line.Origin - Centerpoint;
            double const b = connectionLine * line.Direction;
            double const c = connectionLine * connectionLine - Radius * Radius;

            // ray is outside of sphere and pointing away from sphere
            if (c > 0 && b > 0)
                return std::nullopt;

            double const discriminant = b * b - c;

            // no intersection
            if (discriminant < 0)
                return std::nullopt;

            // calc parameter t on r = line.Origin + line.Direction * t
            double const distance = abs(-b - sqrt(discriminant));

            // intersection point
            // offset a little from hitpoint to avoid collision with this shape on next iteration
            // (this should not be done if collision is disabled for this shape on next iteration)
            Vec3d const intersectionPoint = line.Origin + (distance - 0.001) * line.Direction;

            // calc intersection normal
            Vec3d const normal = (intersectionPoint - Centerpoint).ToNormalized();
            Vec3d const reflectionDirection = Reflect(line.Direction, normal);

            return HitEvent{distance, Line{intersectionPoint, reflectionDirection}};
        }
    };

    struct Plane : public Shape
    {
        Vec3d const Pin;
        Vec3d const Normal;
        Plane(std::string const &label, MaterialInfo const &material, Vec3d pin, Vec3d planeNormal)
            : Shape(label, material), Pin{pin}, Normal{planeNormal}
        {
        }

        std::optional<HitEvent> CheckHit(Line const &line) const
        {
            double const numerator = (Pin - line.Origin) * Normal;
            double const denominator = line.Direction * Normal;
            // parallel?
            if (denominator == 0)
            {
                // if numerator != 0, the ray is within the plane, consider no hit
                return std::nullopt;
            }

            // check numerator / denominator = distance > 0
            double const distance = numerator / denominator;
            if (distance < 0)
                return std::nullopt;

            // Calc reflection ray
            Vec3d const reflectionDirection = Reflect(line.Direction, Normal);
            // offset a little from hitpoint to avoid collision with this shape on next iteration
            // (this should not be done if collision is disabled for this shape on next iteration)
            Vec3d const reflectionPoint = line.Origin + (distance - 0.001) * line.Direction;

            return HitEvent{distance, Line{reflectionPoint, reflectionDirection}};
        }
    };
}