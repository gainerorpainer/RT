#pragma once

#include <random>

#include "Bitmap.h"
#include "Transformation.h"
#include "Camera.h"
#include "Scene.h"

namespace Rt
{
    constexpr unsigned int NUM_RAY_GENERATIONS = 5;
    constexpr unsigned int NUM_DIFFUSE_RAYS = 4;

    class Raytracer
    {

    public:
        Raytracer(unsigned int seed = 1u);

        void RunBitmap(Bitmap::BitmapD &output);

        void RunBitmapParallel(Bitmap::BitmapD &output, unsigned int numSidethreads);

    private:
        std::default_random_engine RngEngine;

        struct RayMarchResult
        {
            ColorD_t ColorFilters;
            ColorD_t Emissions;
        };

        double RandDouble();
        ColorD_t MarchRay(Line const &ray);
    };

}