#pragma once

#include <array>
#include <math.h>

#include "Types.h"

using namespace Types;

namespace Transformation
{
    constexpr double PI = M_PI;

    constexpr double Squared(double dbl)
    {
        return dbl * dbl;
    }

    /// @brief Denotes a transformation from a map (2d) to a sphere, where the center
    /// point of the map gets transformed to the vector (1, 0, 0)
    class Map2Sphere
    {
    private:
        double const LongitudeStep;
        double const LatitudeStep;
        double const MapWidth;
        double const MapHeight;

    public:
        Map2Sphere(unsigned int mapWidth, unsigned int mapHeight, double fov)
            : LongitudeStep{fov / mapWidth},
              LatitudeStep{fov / mapHeight},
              MapWidth{(double)mapWidth},
              MapHeight{(double)mapHeight}
        {
        }

        /// @brief Transforms a point on the map to the sphere
        /// @param x map x coord
        /// @param y map y coord
        /// @return The resulting vector pointing to the sphere point with length 1
        Vec3d_t Transform(unsigned int x, unsigned int y) const
        {
            double const longitude = (x - MapWidth / 2) * LongitudeStep;
            double const latitude = (y - MapHeight / 2) * LongitudeStep;

            return Vec3d_t{cos(latitude) * cos(longitude), cos(latitude) * sin(longitude), sin(latitude)};
        }
    };
}