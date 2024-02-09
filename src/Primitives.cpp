#include "Primitives.h"

namespace Primitives
{
    Vec3d::Vec3d(double x, double y, double z)
    {
        Data[0] = x;
        Data[1] = y;
        Data[2] = z;
    }

    Vec3d::Vec3d(std::array<double, 3> const &vector)
        : Vec3d{vector[0], vector[1], vector[2]}
    {
    }

    Vec3d::Vec3d(Color_t const &color)
        : Vec3d{(double)color[0], (double)color[1], (double)color[2]}
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

    double Vec3d::GetNorm() const
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

    double Vec3d::operator*(const Vec3d &righthand) const
    {
        return X * righthand.X +
               Y * righthand.Y +
               Z * righthand.Z;
    }

    Vec3d Vec3d::operator*(const double righthand) const
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

    Vec3d operator*(double lefthand, const Vec3d &righthand)
    {
        // commutative law
        return righthand * lefthand;
    }
};
