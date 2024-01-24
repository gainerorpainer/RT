#pragma once

#include "Primitives.h"

namespace Shapes
{
    using namespace Primitives;

    class Shape
    {
    public:
        virtual bool IsIntersecting(Line const &line) const = 0;
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

        bool IsIntersecting(Line const &line) const override
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
                return false;
            }
            return true;

            // double t1 = (-B - Math.Sqrt(D)) / (2.0 * A);

            // Point3D solution1 = new Point3D(linePoint0.X * (1 - t1) + t1 * linePoint1.X,
            //                                 linePoint0.Y * (1 - t1) + t1 * linePoint1.Y,
            //                                 linePoint0.Z * (1 - t1) + t1 * linePoint1.Z);
            // if (D == 0)
            // {
            //     return new Point3D[]{solution1};
            // }

            // double t2 = (-B + Math.Sqrt(D)) / (2.0 * A);
            // Point3D solution2 = new Point3D(linePoint0.X * (1 - t2) + t2 * linePoint1.X,
            //                                 linePoint0.Y * (1 - t2) + t2 * linePoint1.Y,
            //                                 linePoint0.Z * (1 - t2) + t2 * linePoint1.Z);

            // // prefer a solution that's on the line segment itself

            // if (Math.Abs(t1 - 0.5) < Math.Abs(t2 - 0.5))
            // {
            //     return new Point3D[]{solution1, solution2};
            // }

            // return new Point3D[]{solution2, solution1};
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