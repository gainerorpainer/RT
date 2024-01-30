#pragma once

#include "Primitives.h"
#include <optional>

namespace Shapes
{
    using namespace Primitives;

    struct HitEvent
    {
        double Distance;
        Vec3d ReflectionDirection;
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
    };

    struct Sphere : public Shape
    {
        Vec3d const Centerpoint;
        double const Radius;

        Sphere(Color_t emission, Vec3d center, double radius)
            : Shape(emission), Centerpoint{center}, Radius{radius}
        {
        }

        std::optional<HitEvent> IsIntersecting(Line const &line) const override
        {
            // http://www.codeproject.com/Articles/19799/Simple-Ray-Tracing-in-C-Part-II-Triangles-Intersec

            double const cx = Centerpoint.X;
            double const cy = Centerpoint.Y;
            double const cz = Centerpoint.Z;

            double const px = line.Origin.X;
            double const py = line.Origin.Y;
            double const pz = line.Origin.Z;

            double const vx = line.Direction.X - px;
            double const vy = line.Direction.Y - py;
            double const vz = line.Direction.Z - pz;

            double const A = vx * vx + vy * vy + vz * vz;
            double const B = 2.0 * (px * vx + py * vy + pz * vz - vx * cx - vy * cy - vz * cz);
            double const C = px * px - 2 * px * cx + cx * cx + py * py - 2 * py * cy + cy * cy +
                             pz * pz - 2 * pz * cz + cz * cz - Radius * Radius;

            // discriminant
            double const D = B * B - 4 * A * C;

            if (D < 0)
            {
                return std::nullopt;
            }

            double const t = (-B - sqrt(D)) / (2.0 * A);
            Vec3d const solution = Vec3d(line.Origin.X * (1 - t) + t * line.Origin.X,
                                    line.Origin.Y * (1 - t) + t * line.Origin.Y,
                                    line.Origin.Z * (1 - t) + t * line.Origin.Z);
            return HitEvent{t, solution};
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

        bool IsIntersecting(Line const &line) const override
        {
            double const numerator = (Pin - line.Origin) * Normal;
            double const denominator = line.Direction * Normal;
            if (denominator == 0)
                // parallel, maybe within plane?
                return numerator == 0;

            // check numerator / denominator = distance > 0
            return (numerator / denominator) > 0;
        }
    };
}