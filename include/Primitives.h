#pragma once

#include <array>
#include <tuple>
#include <math.h>

namespace Primitives
{
    using Color_t = std::array<unsigned char, 3>;
    struct Vec3d;
    using ColorD_t = Vec3d;
    using FloatingType_t = float;

    struct Vec3d
    {
        std::array<FloatingType_t, 3> Data;
        FloatingType_t const &X = Data[0];
        FloatingType_t const &Y = Data[1];
        FloatingType_t const &Z = Data[2];

        /// @brief Ctor
        /// @param x
        /// @param y
        /// @param z
        Vec3d(FloatingType_t x, FloatingType_t y, FloatingType_t z);

        /// @brief Ctor
        /// @param vector from array
        Vec3d(std::array<FloatingType_t, 3> const &vector);

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
        FloatingType_t GetNorm() const;

        /// @brief Compute normalized version of this vector
        /// @return This vector scaled to length 1
        Vec3d ToNormalized() const;

        /// @brief Multiplies X with X, Y with Y, etc
        /// @param other another vector
        /// @return Elementwise mulitplied vector
        Vec3d MultiplyElementwise(const Vec3d &other) const;

        /// @brief Calculates the crossproduct (this) x (other)
        /// @param other another vector
        /// @return Cross product
        Vec3d CrossProd(const Vec3d &other) const;

        /// @brief Applies a somewhat slow rotation within a plane given by two vector
        /// @param planeV1 first plane vector
        /// @param planeV2 second plane vector
        /// @param angle Angle to rotate about (from planeV1 towards planeV2)
        /// @return This rotated vector
        Vec3d RotateAboutPlane(const Vec3d &planeV1, const Vec3d &planeV2, FloatingType_t angle) const;

        /// @brief Applies a somewhat slow rotation about an axis
        /// @param axis Axis of rotation
        /// @param angle Angle of rotation (right hand rule)
        /// @return This rotated vector
        Vec3d RotateAboutAxis(const Vec3d &axis, FloatingType_t angle) const;

        /// @brief Calculates the angle between this and another normalized vector
        /// @param other Normalized vector
        /// @return The angle between this and the other vector
        FloatingType_t
        AngleTo(const Vec3d &other) const;

        /// @brief DOTPRODUCT
        /// @param righthand
        /// @return Elementwise multiplication and summation
        FloatingType_t operator*(const Vec3d &righthand) const;

        /// @brief Scalar multiply
        /// @param righthand scalar
        /// @return Scaled vector
        Vec3d operator*(const FloatingType_t righthand) const;

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
    Vec3d operator*(FloatingType_t lefthand, const Vec3d &righthand);

    struct Line
    {
        Vec3d Origin;
        Vec3d Direction;
    };

    constexpr FloatingType_t Deg2Rad(FloatingType_t deg)
    {
        return deg * M_PI / 180.0;
    }

    constexpr FloatingType_t Rad2Deg(FloatingType_t deg)
    {
        return deg * 180.0 / M_PI;
    }
}
