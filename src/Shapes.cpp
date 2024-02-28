#include "Shapes.h"

#include "Debug.h"

namespace Shapes
{
    constexpr double OFFSET_DELTA = 1e-3;

    double signOf(double val)
    {
        if (val > 0)
            return 1.0;
        else
            return -1.0;
    }

    Vec3d Reflect(Vec3d const &direction, Vec3d const &normal)
    {
        // https://math.stackexchange.com/questions/13261/how-to-get-a-reflection-vector
        Vec3d const reflectionDirection = direction - 2.0 * (direction * normal) * normal;
        DEBUG_ASSERT(AlmostSame(reflectionDirection.GetNorm(), 1.0), "Reflection did not yield normalized vector");

        return reflectionDirection;
    }

    Shape::Shape(std::string const &label, Materials::Material const &material)
        : Label{label}, Material{material}
    {
    }

    Sphere::Sphere(std::string const &label, Materials::Material const &material, Vec3d center, double radius)
        : Shape(label, material), Centerpoint{center}, Radius{radius}
    {
    }

    std::optional<HitEvent> Sphere::CheckHit(Line const &line) const
    {
        DEBUG_ASSERT(AlmostSame(line.Direction.GetNorm(), 1.0), "Line argument not normalized");

        // https://gamedev.stackexchange.com/a/96487

        Vec3d const directionBetween = line.Origin - Centerpoint;
        double const b = directionBetween * line.Direction;
        double const c = directionBetween * directionBetween - Radius * Radius;

        // ray is outside of sphere and pointing away from sphere
        if (c > 0 && b > 0)
            return std::nullopt;

        // no intersection
        double const discriminant = b * b - c;
        if (discriminant < 0)
            return std::nullopt;

        // calc parameter t on r = line.Origin + line.Direction * t, taking the solution that is within the line direction
        // https://www.shadertoy.com/view/4d2XWV
        double distance = -b - signOf(c) * sqrt(discriminant);
        // double const distance = abs(-b - sqrt(discriminant));

        // reflect such that it does not intersect with itself
        DEBUG_ASSERT(distance > 0, "Distance must be in the ray direction");
        distance -= OFFSET_DELTA;

        // intersection point
        Vec3d const intersectionPoint = line.Origin + distance * line.Direction;

        // calc intersection normal
        Vec3d const directionToIntersection = (intersectionPoint - Centerpoint).ToNormalized();
        Vec3d const normal = c > 0 ? directionToIntersection : -directionToIntersection; // takes into consideration when within sphere
        Vec3d const reflectionDirection = Reflect(line.Direction, normal);

        return HitEvent{.DistanceToSurface = distance,
                        .SurfaceNormal = normal,
                        .ReflectedRay = Line{intersectionPoint, reflectionDirection}};
    }

    Plane::Plane(std::string const &label, Materials::Material const &material, Vec3d pin, Vec3d planeNormal)
        // make ctor more accessible by always normalizing normal vector
        : Shape(label, material), Pin{pin}, Normal{planeNormal.ToNormalized()}
    {
    }

    std::optional<HitEvent> Plane::CheckHit(Line const &line) const
    {
        DEBUG_ASSERT(AlmostSame(line.Direction.GetNorm(), 1.0), "Line argument not normalized");

        double const denominator = line.Direction * Normal;
        // parallel?
        if (denominator == 0)
        {
            // if numerator != 0, the ray is within the plane, consider no hit
            DEBUG_ASSERT(!AlmostSame((Pin - line.Origin) * Normal, 0), "Line is within plane");
            return std::nullopt;
        }

        // check numerator / denominator = distance > 0
        double const numerator = (Pin - line.Origin) * Normal;
        double distance = numerator / denominator;
        if (distance < 0)
            return std::nullopt;

        // reflect such that it does not intersect with itself
        DEBUG_ASSERT(distance > 0, "Distance too close!");
        distance -= OFFSET_DELTA;

        // Calc reflection ray
        Vec3d const reflectionDirection = Reflect(line.Direction, Normal);
        Vec3d const reflectionPoint = line.Origin + distance * line.Direction;

        // if (Label == "Floor")
        // {
        //     DEBUG_ASSERT(reflectionPoint.GetNorm() <= 1000, "Reflection outside scene");
        // }

        return HitEvent{.DistanceToSurface = distance,
                        .SurfaceNormal = Normal,
                        .ReflectedRay = Line{reflectionPoint, reflectionDirection}};
    }
}