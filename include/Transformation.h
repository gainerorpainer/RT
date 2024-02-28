#pragma once

#include <array>
#include <math.h>

#include "Primitives.h"

using namespace Primitives;

namespace Transformation
{
    constexpr FloatingType_t PI = M_PI;

    constexpr FloatingType_t Squared(double dbl)
    {
        return dbl * dbl;
    }

    /// @brief Denotes a transformation from a map (2d) to a sphere, where the center
    /// point of the map gets transformed to the vector (1, 0, 0)
    class Map2Sphere
    {
    private:
        FloatingType_t const LongitudeStep;
        FloatingType_t const LatitudeStep;
        FloatingType_t const MapWidth;
        FloatingType_t const MapHeight;

    public:
        Map2Sphere(unsigned int mapWidth, unsigned int mapHeight, FloatingType_t fov)
            : LongitudeStep{fov / mapWidth},
              LatitudeStep{fov / mapHeight},
              MapWidth{(FloatingType_t)mapWidth},
              MapHeight{(FloatingType_t)mapHeight}
        {
        }

        /// @brief Transforms a point on the map to the sphere
        /// @param x map x coord
        /// @param y map y coord
        /// @return The resulting vector pointing to the sphere point with length 1
        Vec3d Transform(unsigned int x, unsigned int y) const
        {
            FloatingType_t const longitude = (x - MapWidth / 2) * LongitudeStep;
            FloatingType_t const latitude = (y - MapHeight / 2) * LongitudeStep;

            return Vec3d{cos(latitude) * cos(longitude), cos(latitude) * sin(longitude), sin(latitude)};
        }
    };
}