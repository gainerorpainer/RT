#include "Primitives.h"

#include "Debug.h"

namespace Primitives
{
    Vec3d::Vec3d(FloatingType_t x, FloatingType_t y, FloatingType_t z)
    {
        Data[0] = x;
        Data[1] = y;
        Data[2] = z;
    }

    Vec3d::Vec3d(std::array<FloatingType_t, 3> const &vector)
        : Vec3d{vector[0], vector[1], vector[2]}
    {
    }

    Vec3d::Vec3d(Color_t const &color)
        : Vec3d{(FloatingType_t)color[0], (FloatingType_t)color[1], (FloatingType_t)color[2]}
    {
    }

    Vec3d::Vec3d()
        : Vec3d{0, 0, 0}
    {
    }

    Vec3d::Vec3d(Vec3d const &other)
        : Vec3d{other.Data}
    {
    }

    void Vec3d::operator=(Vec3d const &other)
    {
        std::copy(other.Data.begin(), other.Data.end(), Data.begin());
    }

    FloatingType_t Vec3d::GetNorm() const
    {
        return sqrt(X * X + Y * Y + Z * Z);
    }

    Vec3d Vec3d::ToNormalized() const
    {
        return *this * (1.0 / GetNorm());
    }

    Vec3d Vec3d::MultiplyElementwise(const Vec3d &other) const
    {
        return Vec3d{X * other.X,
                     Y * other.Y,
                     Z * other.Z};
    }

    Vec3d Vec3d::CrossProd(const Vec3d &other) const
    {
        return Vec3d{
            Y * other.Z - Z * other.Y,
            Z * other.X - X * other.Z,
            X * other.Y - Y * other.X,
        };
    }

    Vec3d Vec3d::RotateAboutPlane(const Vec3d &planeV1, const Vec3d &planeV2, FloatingType_t angle) const
    {
        // https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula

        // construct "k":
        Vec3d const k = planeV1.CrossProd(planeV2).ToNormalized();

        // use rodrigues formula
        return RotateAboutAxis(k, angle);
    }

    Vec3d Vec3d::RotateAboutAxis(const Vec3d &axis, FloatingType_t angle) const
    {
        // https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
        // use rodrigues formula
        return *this * cos(angle) + (axis.CrossProd(*this)) * sin(angle) + axis * (axis * *this) * (1 - cos(angle));
    }

    FloatingType_t Vec3d::AngleTo(const Vec3d &other) const
    {
        DEBUG_ASSERT(AlmostSame(other.GetNorm(), 1.0), "Vector argument not normalized");

        return acos(*this * other);
    }

    FloatingType_t Vec3d::operator*(const Vec3d &righthand) const
    {
        return X * righthand.X +
               Y * righthand.Y +
               Z * righthand.Z;
    }

    Vec3d Vec3d::operator*(const FloatingType_t righthand) const
    {
        return Vec3d{X * righthand, Y * righthand, Z * righthand};
    }

    Vec3d Vec3d::operator+(const Vec3d &righthand) const
    {
        return Vec3d{X + righthand.X,
                     Y + righthand.Y,
                     Z + righthand.Z};
    }

    Vec3d Vec3d::operator-() const
    {
        return Vec3d{-X, -Y, -Z};
    }

    Vec3d Vec3d::operator-(const Vec3d &righthand) const
    {
        return *this + (-righthand);
    }

    Vec3d operator*(FloatingType_t lefthand, const Vec3d &righthand)
    {
        // commutative law
        return righthand * lefthand;
    }
};
