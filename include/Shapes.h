#pragma once

#include <optional>
#include <cassert>

#include "Primitives.h"

namespace Shapes
{
    using namespace Primitives;

    struct HitEvent
    {
        double Distance;
        Line Reflection;
    };

    class Shape
    {
    public:
        virtual std::optional<HitEvent> CheckHit(Line const &line) const = 0;
        Color_t const Emission;

    protected:
        Shape(Color_t color) : Emission{color}
        {
        }

        static Vec3d Reflect(Vec3d const &direction, Vec3d const &normal)
        {
            // https://math.stackexchange.com/questions/13261/how-to-get-a-reflection-vector
            Vec3d const reflectionDirection = direction - 2.0 * (direction * normal) * normal;
            assert(reflectionDirection.GetNorm() == 1.0);

            return reflectionDirection;
        }
    };

    struct Sphere : public Shape
    {
        Vec3d const Centerpoint;
        double const Radius;

        Sphere(Color_t emission, Vec3d center, double radius)
            : Shape(emission), Centerpoint{center}, Radius{radius}
        {
        }

        std::optional<HitEvent> CheckHit(Line const &line) const override
        {
            assert(line.Direction.GetNorm() == 1.0);

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
            double const distance = -b - sqrt(discriminant);

            if (distance < 0)
                // inside sphere, bad
                exit(-1);

            // intersection point
            Vec3d const intersectionPoint = line.Origin + distance * line.Direction;

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
        Plane(Color_t emission, Vec3d pin, Vec3d planeNormal)
            : Shape(emission), Pin{pin}, Normal{planeNormal}
        {
        }

        std::optional<HitEvent> CheckHit(Line const &line) const
        {
            double const numerator = (Pin - line.Origin) * Normal;
            double const denominator = line.Direction * Normal;
            // parallel
            if (denominator == 0)
            {
                // line is within plane, bad
                if (numerator == 0)
                    exit(-1);

                return std::nullopt;
            }

            // check numerator / denominator = distance > 0
            double const distance = numerator / denominator;
            if (distance < 0)
                return std::nullopt;

            // Calc reflection ray
            Vec3d const reflectionDirection = Reflect(line.Direction, Normal);
            Vec3d const reflectionPoint = line.Origin + distance * line.Direction; 
            return HitEvent{distance, Line{reflectionPoint, reflectionDirection}};
        }
    };
}