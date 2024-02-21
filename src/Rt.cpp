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

    constexpr unsigned int constexpr_pow(unsigned int base, unsigned int power)
    {
        unsigned int multiplier = base;
        for (size_t i = 0; i < power - 1; i++)
            multiplier *= base;
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
            double ParentWeight;
            double Weight;
            Vec3d ColorFilter;
            Shapes::Shape const *OriginShape;
        };
        struct generation_t
        {
            std::array<generationElement_t, constexpr_pow(NUM_DIFFUSE_RAYS, NUM_RAY_GENERATIONS)> Elements;
            unsigned int Count;
        };
        std::array<generation_t, 2> generationBuffer{};
        {
            size_t const NUM_GENERATION_ELEMENTS = generationBuffer[0].Elements.size();
            size_t const STACK_SIZE_KB = sizeof(generationBuffer) / 1024u;
            static_assert(STACK_SIZE_KB < 512, "Critical Stack Size");
        }
        Vec3d emissionAccumulator{0, 0, 0};

        // prepare initial conditions
        generationBuffer[0].Count = 1;
        generationBuffer[0].Elements[0] = generationElement_t{
            .Ray = ray,
            .ParentWeight = 1.0,
            .Weight = 1.0,
            .ColorFilter = Vec3d{1, 1, 1},
            .OriginShape = nullptr};

        for (size_t generationIndex = 0; generationIndex < NUM_RAY_GENERATIONS + 1; generationIndex++)
        {
            // swaps buffers on each iteration automatically
            generation_t const &lastGeneration = generationBuffer[generationIndex % 2];
            generation_t &nextGeneration = generationBuffer[(1 + generationIndex) % 2];

            // "clear" next generation
            nextGeneration.Count = 0;

            for (size_t i = 0; i < lastGeneration.Count; i++)
            {
                generationElement_t const &parentElement = lastGeneration.Elements[i];
                auto const nearest = GetClosestIntersection(parentElement.Ray, parentElement.OriginShape);

                if (!nearest.Shape)
                {
                    // nothing hit, bad?
                    DEBUG_WARN("Ray did not hit anything!");
                    continue;
                }

                Materials::Material const &material = nearest.Shape->Material;

                // check if light source was hit
                if (material.IsLightsource)
                {
                    emissionAccumulator = emissionAccumulator + parentElement.Weight * material.Emission.MultiplyElementwise(parentElement.ColorFilter);
                    continue;
                }

                // do not generate more rays on last iteration as they will not be checked anymore
                if (generationIndex == NUM_RAY_GENERATIONS)
                    break;

                Vec3d const effectiveColor = parentElement.ColorFilter.MultiplyElementwise(material.ColorFilter);

                // perfect mirror will only spawn single ray
                nextGeneration.Elements[nextGeneration.Count] = generationElement_t{
                    .Ray = nearest.Hitevent.ReflectedRay,
                    .ParentWeight = parentElement.Weight,
                    .Weight = 1.0 - material.DiffusionFactor,
                    .ColorFilter = effectiveColor,
                    .OriginShape = nearest.Shape};
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

                    nextGeneration.Elements[nextGeneration.Count] = generationElement_t{
                        .Ray = probingRay,
                        .ParentWeight = parentElement.Weight,
                        // both have norm = 1! So this weights parallel lines to 1 and perpendicular to 0
                        .Weight = abs(nearest.Hitevent.SurfaceNormal * probingRay.Direction),
                        .ColorFilter = effectiveColor,
                        .OriginShape = nearest.Shape};
                    nextGeneration.Count++;
                };
            }

            // do not convert weights as they will not be read anymore
            if (generationIndex == NUM_RAY_GENERATIONS)
                break;

            // make "local abs" to "global rel" weights
            if (nextGeneration.Count < 2)
            {
                nextGeneration.Elements[0].Weight = 1.0;
            }
            else
            {
                double const weightSum = std::accumulate(nextGeneration.Elements.begin(), nextGeneration.Elements.begin() + nextGeneration.Count,
                                                         0.0, [](double const &last, generationElement_t const &next)
                                                         { return last + next.Weight; });
                for (size_t i = 0; i < nextGeneration.Count; i++)
                {
                    double const localWeight = nextGeneration.Elements[i].Weight / weightSum;
                    nextGeneration.Elements[i].Weight = nextGeneration.Elements[i].ParentWeight * localWeight;
                }
            }
        }

        return emissionAccumulator;
    }

    double Raytracer::RandDouble()
    {
        return std::uniform_real_distribution<double>{}(RngEngine);
    }
}