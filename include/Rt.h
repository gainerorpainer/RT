#pragma once

#include <random>

#include "Bitmap.h"
#include "Transformation.h"
#include "Camera.h"
#include "Scene.h"

namespace Rt
{
    constexpr unsigned int BOUNCE_ITERATIONS = 4;
    constexpr unsigned int DIFFUSE_RAYS = 4;

    class Raytracer
    {

    public:
        Raytracer(unsigned int seed = 1u);

        void RunBitmap(Bitmap::BitmapD &output);

    private:
        std::default_random_engine RngEngine;

        struct RayMarchResult
        {
            ColorD_t ColorFilters;
            ColorD_t Emissions;
        };

        double RandDouble();
        RayMarchResult MarchRay(Line const &ray, unsigned int const recursionDepth = 0);
    };

}