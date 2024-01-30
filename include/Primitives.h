#pragma once

#include <array>
#include <tuple>
#include <math.h>

namespace Primitives
{
    using Color_t = std::array<unsigned char, 3>;

    struct Vec3d
    {
        std::array<double, 3> const Vector;
        double const &X = Vector[0];
        double const &Y = Vector[1];
        double const &Z = Vector[2];

        /// @brief Ctor
        /// @param x
        /// @param y
        /// @param z
        Vec3d(double x, double y, double z)
            : Vector{{x, y, z}},
              X{Vector[0]},
              Y{Vector[1]},
              Z{Vector[2]}
        {
        }

        /// @brief Ctor
        /// @param vector from array
        Vec3d(std::array<double, 3> vector)
            : Vec3d{vector[0], vector[1], vector[2]}
        {
        }

        /// @brief Default Ctor
        Vec3d()
        : Vec3d{0, 0, 0}
        {
        }

        /// @brief Copy Ctor
        /// @param other from other instance
        Vec3d(Vec3d const &other)
        : Vec3d{other.Vector}
        {
        }

        /// @brief DOTPRODUCT
        /// @param righthand
        /// @return Elementwise multiplication and summation
        double operator*(const Vec3d &righthand) const
        {
            return Vector[0] * righthand.Vector[0] +
                   Vector[1] * righthand.Vector[1] +
                   Vector[2] * righthand.Vector[2];
        }

        /// @brief MINUS
        /// @param righthand
        /// @return Elementwise minus
        Vec3d
        operator-(const Vec3d &righthand) const
        {
            return Vec3d{Vector[0] - righthand.Vector[0],
                         Vector[1] - righthand.Vector[1],
                         Vector[2] - righthand.Vector[2]};
        }
    };

    struct Line
    {
        Vec3d const Origin;
        Vec3d const Direction;

        Line(Vec3d origin, Vec3d direction)
            : Origin{origin}, Direction{direction}
        {
        }
    };

    constexpr double Deg2Rad(double deg)
    {
        return deg * M_PI / 180.0;
    }

    constexpr double Rad2Deg(double deg)
    {
        return deg * 180.0 / M_PI;
    }
}
