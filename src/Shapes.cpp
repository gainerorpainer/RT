#include "Shapes.h"

#include "Debug.h"

namespace Shapes
{
    constexpr FloatingType_t OFFSET_DELTA = 1e-3;

    FloatingType_t signOf(FloatingType_t val)
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

    Shape::Shape(std::string const &label)
        : Label{label}
    {
    }

    Sphere::Sphere(std::string const &label, Materials::Material const &material, Vec3d center, FloatingType_t radius)
        : Shape(label), Material{material}, Centerpoint{center}, Radius{radius}
    {
    }

    std::optional<HitEvent> Sphere::CheckHit(Line const &line) const
    {
        DEBUG_ASSERT(AlmostSame(line.Direction.GetNorm(), 1.0), "Line argument not normalized");

        // https://gamedev.stackexchange.com/a/96487

        Vec3d const directionBetween = line.Origin - Centerpoint;
        FloatingType_t const b = directionBetween * line.Direction;
        FloatingType_t const c = directionBetween * directionBetween - Radius * Radius;

        // ray is outside of sphere and pointing away from sphere
        if (c > 0 && b > 0)
            return std::nullopt;

        // no intersection
        FloatingType_t const discriminant = b * b - c;
        if (discriminant < 0)
            return std::nullopt;

        // calc parameter t on r = line.Origin + line.Direction * t, taking the solution that is within the line direction
        // https://www.shadertoy.com/view/4d2XWV
        FloatingType_t distance = -b - signOf(c) * sqrt(discriminant);
        // FloatingType_t const distance = abs(-b - sqrt(discriminant));

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

    Materials::Material const &Sphere::GetMaterial(HitEvent const &) const
    {
        return Material;
    }

    Plane::Plane(std::string const &label, Materials::Material const &material, Vec3d pin, Vec3d planeNormal)
        // make ctor more accessible by always normalizing normal vector
        : Shape(label), Material{material}, Pin{pin}, Normal{planeNormal.ToNormalized()}
    {
    }

    Plane::Plane(std::string const &label, Vec3d pin, Vec3d planeNormal)
        : Plane(label, Materials::Material{}, pin, planeNormal)
    {
    }

    std::optional<HitEvent> Plane::CheckHit(Line const &line) const
    {
        DEBUG_ASSERT(AlmostSame(line.Direction.GetNorm(), 1.0), "Line argument not normalized");

        FloatingType_t const denominator = line.Direction * Normal;
        // parallel?
        if (denominator == 0)
        {
            // if numerator != 0, the ray is within the plane, consider no hit
            DEBUG_ASSERT(!AlmostSame((Pin - line.Origin) * Normal, 0), "Line is within plane");
            return std::nullopt;
        }

        // check numerator / denominator = distance > 0
        FloatingType_t const numerator = (Pin - line.Origin) * Normal;
        FloatingType_t distance = numerator / denominator;
        if (distance < 0)
            return std::nullopt;

        // reflect such that it does not intersect with itself
        DEBUG_ASSERT(distance > 0, "Distance too close!");
        distance -= OFFSET_DELTA;

        // Calc reflection ray
        Vec3d const reflectionDirection = Reflect(line.Direction, Normal);
        Vec3d const reflectionPoint = line.Origin + distance * line.Direction;

        return HitEvent{.DistanceToSurface = distance,
                        .SurfaceNormal = Normal,
                        .ReflectedRay = Line{reflectionPoint, reflectionDirection}};
    }

    Materials::Material const &Plane::GetMaterial(HitEvent const &) const
    {
        return Material;
    }

    CheckerboardPlane::CheckerboardPlane(std::string const &label, std::pair<Materials::Material, Materials::Material> materials, FloatingType_t width, Vec3d pin, Vec3d normal)
        : Plane(label, pin, normal), Materials{materials}, Width{width}
    {
    }

    Materials::Material const &CheckerboardPlane::GetMaterial(HitEvent const &hitEvent) const
    {
        return (abs(std::fmod(hitEvent.ReflectedRay.Origin.X, Width)) < (Width / (FloatingType_t)2)) && (abs(std::fmod(hitEvent.ReflectedRay.Origin.Y + Width / (FloatingType_t)2, 2)) < (Width / (FloatingType_t)2))
                   ? Materials.first
                   : Materials.second;
    }
}
