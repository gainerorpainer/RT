#include "Rt.h"

#include <algorithm>
#include <thread>

#include "Debug.h"

namespace Rt
{
    // see if ray intersects any object
    struct rayIntersection_t
    {
        Shapes::Shape const *Shape;
        Shapes::HitEvent Hitevent;
    };

    constexpr unsigned int constexpr_pow(unsigned int diffuseRays, unsigned int iterations)
    {
        unsigned int multiplier = diffuseRays;
        for (size_t i = 0; i < iterations - 1; i++)
            multiplier *= multiplier;
        return multiplier;
    }

    Raytracer::Raytracer(unsigned int seed)
        : RngEngine{seed}
    {
    }

    void Raytracer::RunBitmap(Bitmap::BitmapD &output)
    {
        Transformation::Map2Sphere const cameraTransformation{Bitmap::BITMAP_WIDTH, Bitmap::BITMAP_HEIGHT, Camera::FOV};
        unsigned int lastProgress = 0;
        for (size_t y = 0; y < Bitmap::BITMAP_HEIGHT; y++)
        {
            for (size_t x = 0; x < Bitmap::BITMAP_WIDTH; x++)
            {
                // first ray comes from cam
                Line ray = {Camera::Origin, cameraTransformation.Transform(x, y)};

                // Let ray bounce around and determine the color
                auto const pixelcolor = MarchRay(ray);

                // paint pixel with object color into *image space*
                std::copy(pixelcolor.Data.begin(), pixelcolor.Data.end(), output.atPixel(x, y));
            }

            unsigned int progress = y * 100u / Bitmap::BITMAP_HEIGHT;
            if (progress != lastProgress)
            {
                lastProgress = progress;
                DEBUG_PRINT(std::to_string(progress) + "%");
            }
        }
    }

    void Raytracer::RunBitmapParallel(Bitmap::BitmapD &output, unsigned int numSidethreads)
    {
        // spawn threads
        std::vector<std::thread> sidethreads;
        size_t argFromX = 0;
        size_t argFromY = 0;
        size_t const xStep = (size_t)ceil((double)Bitmap::BITMAP_WIDTH / (double)numSidethreads);
        size_t const yStep = (size_t)ceil((double)Bitmap::BITMAP_HEIGHT / (double)numSidethreads);
        for (size_t i = 0; i < numSidethreads; i++)
        {
            size_t const argToX = std::min((size_t)Bitmap::BITMAP_WIDTH, argFromX + xStep);
            size_t const argToY = std::min((size_t)Bitmap::BITMAP_HEIGHT, argFromY + yStep);

            // run as sidethread
            sidethreads.emplace_back([this, &output](size_t fromY, size_t toY, size_t argFromX, size_t argToX)
                                     {
                                        Transformation::Map2Sphere const cameraTransformation{Bitmap::BITMAP_WIDTH, Bitmap::BITMAP_HEIGHT, Camera::FOV};
                                        for (size_t y = fromY; y < toY; y++)
                                        {
                                            for (size_t x = argFromX; x < argToX; x++)
                                            {
                                                // first ray comes from cam
                                                Line ray = {Camera::Origin, cameraTransformation.Transform(x, y)};

                                                // Let ray bounce around and determine the color
                                                ColorD_t const pixelcolor = MarchRay(ray);

                                                // paint pixel with object color into *image space*
                                                std::copy(pixelcolor.Data.begin(), pixelcolor.Data.end(), output.atPixel(x, y));
                                            }
                                        } },
                                     argFromY, argToY, 0, Bitmap::BITMAP_WIDTH);
            argFromX += xStep;
            argFromY += yStep;
        }

        // wait for completion
        for (auto &thread : sidethreads)
            thread.join();
    }

    inline rayIntersection_t GetClosestIntersection(Line const &ray, Shapes::Shape const *disabledCollision)
    {
        std::array<rayIntersection_t, Scene::Objects.size()> possibleHits = {};
        unsigned int hitCount = 0;
        for (unsigned int i = 0; i < Scene::Objects.size(); i++)
        {
            Shapes::Shape const *shape = Scene::Objects[i];

            if (shape == disabledCollision)
                continue;

            auto hitevent = shape->CheckHit(ray);
            if (!hitevent)
                continue;

            possibleHits[hitCount++] = rayIntersection_t{shape, hitevent.value()};
        }

        if (hitCount == 0)
            return rayIntersection_t{};

        // find element with shortest distance
        double shortestDistance = INFINITY;
        decltype(possibleHits)::iterator nearest = possibleHits.end();
        for (auto it = possibleHits.begin(); it < possibleHits.begin() + hitCount; it++)
        {
            if (!it->Shape)
                continue;

            if (it->Hitevent.DistanceToSurface >= shortestDistance)
                continue;

            shortestDistance = it->Hitevent.DistanceToSurface;
            nearest = it;
        }

        return *nearest;
    }

    ColorD_t Raytracer::MarchRay(Line const &ray)
    {
        struct generationElement_t
        {
            Line Ray;
            Shapes::Shape const *OriginShape;
        };
        struct generation_t
        {
            std::array<generationElement_t, constexpr_pow(NUM_RAY_GENERATIONS, NUM_DIFFUSE_RAYS)> Elements;
            unsigned int Count;
        };
        std::array<generation_t, 2> generationBuffer{};
        Vec3d emissionAccumulator{0, 0, 0};
        Vec3d filterAccumulator{1, 1, 1};
        unsigned int recursionDepth = 0;

        // prepare initial conditions
        generationBuffer[0].Count = 1;
        generationBuffer[0].Elements[0].Ray = ray;

        for (size_t generationIndex = 0; generationIndex < NUM_RAY_GENERATIONS + 1; generationIndex++)
        {
            // swaps buffers on each iteration automatically
            generation_t const & lastGeneration = generationBuffer[generationIndex % 2];
            generation_t & nextGeneration = generationBuffer[(1 + generationIndex) % 2];

            // "clear" next generation
            nextGeneration.Count = 0;

            for (size_t i = 0; i < lastGeneration.Count; i++)
            {
                auto const nearest = GetClosestIntersection(lastGeneration.Elements[i].Ray, lastGeneration.Elements[i].OriginShape);

                if (!nearest.Shape)
                {
                    // nothing hit, bad?
                    DEBUG_WARN("Ray did not hit anything!");
                    continue;
                }

                // todo: apply material to accumulator
                Materials::Material const &material = nearest.Shape->Material;

                // check if light source was hit
                if (material.IsLightsource)
                {
                    // todo: add weight
                    emissionAccumulator = emissionAccumulator + material.Emission;
                    continue;
                }

                // do not generate more rays on last iteration as they will not be checked anymore
                if (generationIndex == NUM_RAY_GENERATIONS)
                    break;

                // perfect mirror will only spawn single ray
                nextGeneration.Elements[nextGeneration.Count].Ray = nearest.Hitevent.ReflectedRay;
                nextGeneration.Elements[nextGeneration.Count].OriginShape = nearest.Shape;
                // .Weight = (1.0 - material.DiffusionFactor)
                nextGeneration.Count++;

                // total reflection: Material then acts like a perfect mirror
                double const angleOfIncidence = abs(nearest.Hitevent.ReflectedRay.Direction.AngleTo(nearest.Hitevent.SurfaceNormal));
                bool const isTotallyReflected = angleOfIncidence > material.CriticalAngle;

                if (material.DiffusionFactor < 0.01 || isTotallyReflected)
                    continue;

                // spawn random rays (1 is already spawned)
                for (size_t j = 1; j < NUM_DIFFUSE_RAYS; j++)
                {
                    // Start from surface normal
                    Line probingRay{nearest.Hitevent.ReflectedRay.Origin, nearest.Hitevent.SurfaceNormal};

                    // rotate away from surface normal in the plane (surface normal) x (reflection)
                    probingRay.Direction = probingRay.Direction.RotateAboutPlane(nearest.Hitevent.SurfaceNormal, nearest.Hitevent.ReflectedRay.Direction, RandDouble() * Deg2Rad(60));

                    // start rotating about the normal in appropriate steps
                    probingRay.Direction = probingRay.Direction.RotateAboutAxis(nearest.Hitevent.SurfaceNormal, RandDouble() * Deg2Rad(360));

                    DEBUG_ASSERT(AlmostSame(probingRay.Direction.GetNorm(), 1.0), "Rotation is bad for vector");

                    nextGeneration.Elements[nextGeneration.Count].Ray = probingRay;
                    nextGeneration.Elements[nextGeneration.Count].OriginShape = nearest.Shape;
                    // todo: add weight
                    // both have norm = 1! So this weights parallel lines to 1 and perpendicular to 0
                    // .Weight = abs(nearest.Hitevent.SurfaceNormal * probingRay.Direction)
                    nextGeneration.Count++;
                };
            }
        }


        ColorD_t const result = emissionAccumulator.MultiplyElementwise(filterAccumulator);
        return result;
    }

    double Raytracer::RandDouble()
    {
        return std::uniform_real_distribution<double>{}(RngEngine);
    }
}