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
        size_t const xStep = (size_t)ceil((FloatingType_t)Bitmap::BITMAP_WIDTH / (FloatingType_t)numSidethreads);
        size_t const yStep = (size_t)ceil((FloatingType_t)Bitmap::BITMAP_HEIGHT / (FloatingType_t)numSidethreads);
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

    inline rayIntersection_t GetClosestIntersection(Line const &ray)
    {
        std::array<rayIntersection_t, Scene::Objects.size()> possibleHits = {};
        unsigned int hitCount = 0;
        for (unsigned int i = 0; i < Scene::Objects.size(); i++)
        {
            Shapes::Shape const *shape = Scene::Objects[i];

            auto hitevent = shape->CheckHit(ray);
            if (!hitevent)
                continue;

            possibleHits[hitCount++] = rayIntersection_t{shape, hitevent.value()};
        }

        if (hitCount == 0)
            return rayIntersection_t{};

        // find element with shortest distance
        FloatingType_t shortestDistance = INFINITY;
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
            uint16_t ParentRayIndex;
            float ParentWeight;
            float Weight;
            Vec3d ColorFilter;
        };
        struct generation_t
        {
            std::array<generationElement_t, constexpr_pow(NUM_DIFFUSE_RAYS, NUM_RAY_GENERATIONS)> Elements;
            uint16_t Count;
        };
        struct parentRays_t
        {
            std::array<Line, constexpr_pow(NUM_DIFFUSE_RAYS, NUM_RAY_GENERATIONS - 1)> Rays;
            uint16_t Count;
        };
        std::array<generation_t, 2> generationBuffer{};
        std::array<parentRays_t, 2> generationRays{};
        Vec3d emissionAccumulator{0, 0, 0};
        FloatingType_t weightSum;

        // asserts
        {
            size_t const STACK_SIZE_KB = (sizeof(generationBuffer) + sizeof(generationRays)) / 1024u;
            static_assert(STACK_SIZE_KB < 1024, "Critical Stack Size");
            static_assert(constexpr_pow(2, sizeof(generation_t::Count) * 8) > generationBuffer[0].Elements.size(), "Count is too small");
            static_assert(constexpr_pow(2, sizeof(parentRays_t::Count) * 8) > generationRays[0].Rays.size(), "Count is too small");
            static_assert(constexpr_pow(2, sizeof(generationElement_t::ParentRayIndex) * 8) > generationRays[0].Rays.size(), "ParentRayIndex is too small");
        }

        // prepare initial conditions
        generationRays[0].Rays[0] = ray;
        generationBuffer[0].Count = 1;
        generationBuffer[0].Elements[0] = generationElement_t{
            .ParentRayIndex = 0,
            .ParentWeight = 1.0,
            .Weight = 1.0,
            .ColorFilter = Vec3d{1, 1, 1}};

        for (size_t generationIndex = 0; generationIndex < NUM_RAY_GENERATIONS + 1; generationIndex++)
        {
            // swaps buffers on each iteration automatically
            generation_t const &lastGeneration = generationBuffer[generationIndex % 2];
            generation_t &nextGeneration = generationBuffer[(1 + generationIndex) % 2];

            parentRays_t const &lastRays = generationRays[generationIndex % 2];
            parentRays_t &nextRays = generationRays[(1 + generationIndex) % 2];

            // "clear" next generation
            nextGeneration.Count = 0;
            nextRays.Count = 0;
            weightSum = 0;

            for (size_t i = 0; i < lastGeneration.Count; i++)
            {
                generationElement_t const &parentElement = lastGeneration.Elements[i];

                // skip non-significant elements
                if (parentElement.Weight < 0.01)
                    continue;

                auto const nearest = GetClosestIntersection(lastRays.Rays[parentElement.ParentRayIndex]);

                if (!nearest.Shape)
                {
                    // nothing hit, bad?
                    DEBUG_WARN("Ray did not hit anything!");
                    continue;
                }

                Materials::Material const &material = nearest.Shape->GetMaterial(nearest.Hitevent);

                // check if light source was hit
                if (material.IsLightsource)
                {
                    emissionAccumulator = emissionAccumulator + parentElement.Weight * material.Emission.MultiplyElementwise(parentElement.ColorFilter);
                    continue;
                }

                // todo, add some light in the color of the material
                // emissionAccumulator = emissionAccumulator + (255 * material.ColorFilter);

                // do not generate more rays on last iteration as they will not be checked anymore
                if (generationIndex == NUM_RAY_GENERATIONS)
                    continue;

                Vec3d const effectiveColor = parentElement.ColorFilter.MultiplyElementwise(material.ColorFilter);

                // too dark
                if (effectiveColor.GetNorm() < 0.01)
                    continue;

                FloatingType_t angleOfIncidence = abs(nearest.Hitevent.ReflectedRay.Direction.AngleTo(nearest.Hitevent.SurfaceNormal));
                if (isnan(angleOfIncidence))
                    // bad bahaviour when angle gets small
                    angleOfIncidence = 0;

                // lerp between diffusion factor and 0 between the range critical angle .. 90°
                FloatingType_t const apparentDiffusionFactor = angleOfIncidence < material.CriticalAngle ? material.DiffusionFactor
                                                                                                         : std::lerp(material.DiffusionFactor, 0.0,
                                                                                                                     (angleOfIncidence - material.CriticalAngle) / (Deg2Rad(90) - material.CriticalAngle));
                DEBUG_ASSERT(apparentDiffusionFactor >= 0 && apparentDiffusionFactor <= material.DiffusionFactor, "Bad diffusion fadeout");

                // perfect mirror will only spawn single ray
                FloatingType_t const weight = FloatingType_t{1} - apparentDiffusionFactor;
                // too little contribution
                if (weight > 0.01)
                {
                    nextRays.Rays[nextRays.Count] = nearest.Hitevent.ReflectedRay;
                    nextGeneration.Elements[nextGeneration.Count] = generationElement_t{
                        .ParentRayIndex = nextRays.Count,
                        .ParentWeight = parentElement.Weight,
                        .Weight = (FloatingType_t{1} - apparentDiffusionFactor) * (FloatingType_t)0.3,
                        .ColorFilter = effectiveColor};
                    nextRays.Count++;
                    nextGeneration.Count++;
                    weightSum += weight;
                }

                // mirror material or total reflection
                if ((apparentDiffusionFactor < 0.01) || (angleOfIncidence > material.CriticalAngle))
                    continue;

                // spawn random rays (1 is already spawned)
                for (size_t j = 1; j < NUM_DIFFUSE_RAYS; j++)
                {
                    // Start from reflection
                    Line probingRay {nearest.Hitevent.ReflectedRay.Origin, nearest.Hitevent.SurfaceNormal};

                    // rotate away from surface normal in the plane (surface normal) x (reflection)
                    probingRay.Direction = probingRay.Direction.RotateAboutPlane(nearest.Hitevent.SurfaceNormal, nearest.Hitevent.ReflectedRay.Direction, RandFloat() * Deg2Rad(60));

                    // start rotating about the normal in appropriate steps
                    probingRay.Direction = probingRay.Direction.RotateAboutAxis(nearest.Hitevent.SurfaceNormal, RandFloat() * Deg2Rad(360));

                    DEBUG_ASSERT(AlmostSame(probingRay.Direction.GetNorm(), 1.0), "Rotation is bad for vector");

                    FloatingType_t const weight = abs(nearest.Hitevent.SurfaceNormal * probingRay.Direction);

                    // too little contribution
                    if (weight < 0.01)
                        continue;

                    nextRays.Rays[nextRays.Count] = probingRay;
                    nextGeneration.Elements[nextGeneration.Count] = generationElement_t{
                        .ParentRayIndex = nextRays.Count,
                        .ParentWeight = parentElement.Weight,
                        // both have norm = 1! So this weights parallel lines to 1 and perpendicular to 0
                        .Weight = weight,
                        .ColorFilter = effectiveColor};
                    nextRays.Count++;
                    nextGeneration.Count++;
                    weightSum += weight;
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
                for (size_t i = 0; i < nextGeneration.Count; i++)
                {
                    FloatingType_t const localWeight = nextGeneration.Elements[i].Weight / weightSum;
                    nextGeneration.Elements[i].Weight = nextGeneration.Elements[i].ParentWeight * localWeight;
                }
            }
        }

        return emissionAccumulator;
    }

    FloatingType_t Raytracer::RandFloat()
    {
        return std::uniform_real_distribution<FloatingType_t>{}(RngEngine);
    }
}