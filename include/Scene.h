#pragma once

#include <array>

#include "Types.h"

using namespace Types;

namespace Scene
{
    struct Sphere
    {
        Color_t const Color;
        Vec3d_t const Centerpoint;
        double const Radius;

        bool IsIntersecting(Line_t const & line) const
        {
            // http://www.codeproject.com/Articles/19799/Simple-Ray-Tracing-in-C-Part-II-Triangles-Intersec

            double const cx = Centerpoint[0];
            double const cy = Centerpoint[1];
            double const cz = Centerpoint[2];

            auto const & lineOrigin = std::get<0>(line);
            double const px =  lineOrigin[0];
            double const py = lineOrigin[1];
            double const pz = lineOrigin[2];

            auto const & lineDirection = std::get<1>(line);
            double const vx = lineDirection[0] - px;
            double const vy = lineDirection[1] - py;
            double const vz = lineDirection[2] - pz;

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

    std::array<Sphere, 1> Objects = {Sphere{.Color = {255, 0, 0}, .Centerpoint = {2, 0, 0}, .Radius = 1}};
}
