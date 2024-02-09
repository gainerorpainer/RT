#pragma once

#include <array>
#include <tuple>
#include <math.h>

namespace Primitives
{
    using Color_t = std::array<unsigned char, 3>;
    struct Vec3d;
    using ColorD_t = Vec3d;

    struct Vec3d
    {
        std::array<double, 3> Data;
        double const &X = Data[0];
        double const &Y = Data[1];
        double const &Z = Data[2];

        /// @brief Ctor
        /// @param x
        /// @param y
        /// @param z
        Vec3d(double x, double y, double z);

        /// @brief Ctor
        /// @param vector from array
        Vec3d(std::array<double, 3> const &vector);

        /// @brief Ctor
        /// @param color from color
        Vec3d(Color_t const &color);

        /// @brief Default Ctor
        Vec3d();

        /// @brief Copy Ctor
        /// @param other from other instance
        Vec3d(Vec3d const &other);

        /// @brief Assignment operator
        /// @param other from other instance
        void operator=(Vec3d const &other);

        template <typename T>
        std::array<T, 3> Cast() const;

        /// @brief Compute norm (length) of vector
        /// @return The norm (length)
        double GetNorm() const;

        /// @brief Compute normalized version of this vector
        /// @return This vector scaled to length 1
        Vec3d ToNormalized() const;

        /// @brief Multiplies X with X, Y with Y, etc
        /// @param other another vector
        /// @return Elementwise mulitplied vector
        Vec3d MultiplyElementwise(const Vec3d &other) const;

        /// @brief DOTPRODUCT
        /// @param righthand
        /// @return Elementwise multiplication and summation
        double operator*(const Vec3d &righthand) const;

        /// @brief Scalar multiply
        /// @param righthand scalar
        /// @return Scaled vector
        Vec3d operator*(const double righthand) const;

        /// @brief PLUS
        /// @param righthand
        /// @return Elementwise plus
        Vec3d operator+(const Vec3d &righthand) const;

        /// @brief Inversion
        /// @return negative vector
        Vec3d operator-() const;

        /// @brief MINUS
        /// @param righthand
        /// @return Elementwise minus
        Vec3d operator-(const Vec3d &righthand) const;
    };

    template <typename T>
    inline std::array<T, 3> Vec3d::Cast() const
    {
        return std::array<T, 3>{(T)Data[0], (T)Data[1], (T)Data[2]};
    }

    /// @brief Scalar multiplication (lefthanded)
    /// @param lefthand scalar
    /// @param righthand vector
    /// @return scaled vector
    Vec3d operator*(double lefthand, const Vec3d &righthand);

    struct Line
    {
        Vec3d Origin;
        Vec3d Direction;
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
