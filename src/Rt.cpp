#include "Rt.h"

#include <algorithm>
#include <thread>
#include <cmath>

#include "Debug.h"

namespace Rt
{
    struct RayIntersection_t
    {
        Shapes::Shape const *Shape;
        Shapes::HitEvent Hitevent;
    };

    Raytracer::Raytracer(unsigned int seed)
        : RngEngine{seed}
    {
    }

    constexpr unsigned int GetTreesize(unsigned int diffuseRays, unsigned int iterations)
    {
        unsigned int multiplier = diffuseRays;
        for (size_t i = 0; i < iterations - 1; i++)
            multiplier *= multiplier;
        return multiplier;
    }

    void Raytracer::RunBitmap(Bitmap::BitmapD &output)
    {
        Transformation::Map2Sphere const cameraTransformation{Bitmap::STD_BITMAP_WIDTH, Bitmap::STD_BITMAP_HEIGHT, Camera::FOV};
        for (size_t y = 0; y < Bitmap::STD_BITMAP_HEIGHT; y++)
        {
            for (size_t x = 0; x < Bitmap::STD_BITMAP_WIDTH; x++)
            {
                // first ray comes from cam
                Line ray = {Camera::Origin, cameraTransformation.Transform(x, y)};

                // Let ray bounce around and determine the color
                auto const raymarch = MarchRay(ray);

                // apply color filters on the emmision spectrum
                Color_t const pixelcolor = raymarch.Emissions.MultiplyElementwise(raymarch.ColorFilters).Cast<unsigned char>();

                // paint pixel with object color into *image space*
                std::copy(pixelcolor.begin(), pixelcolor.end(), output.atPixel(x, y));
            } 
        }
    }

    void Raytracer::RunBitmapParallel(Bitmap::BitmapD &output, unsigned int availableCpuCores)
    {

        unsigned int const numSidethreads = std::max((unsigned int)(sizeof(output) / (1024ULL * 1024ULL)), availableCpuCores);

        // spawn threads
        std::vector<std::thread> sidethreads;
        size_t argFromY = 0;
        size_t const yStep = (size_t)ceil((double)Bitmap::STD_BITMAP_HEIGHT / (double)numSidethreads);
        for (size_t i = 0; i < numSidethreads; i++)
        {
            size_t const argToY = std::min((size_t)Bitmap::STD_BITMAP_HEIGHT, argFromY + yStep);

            // run as sidethread
            sidethreads.emplace_back([this, &output](size_t fromY, size_t toY)
                                     {
                                        Transformation::Map2Sphere const cameraTransformation{Bitmap::STD_BITMAP_WIDTH, Bitmap::STD_BITMAP_HEIGHT, Camera::FOV};
                                        for (size_t y = fromY; y < toY; y++)
                                        {
                                            for (size_t x = 0; x < Bitmap::STD_BITMAP_WIDTH; x++)
                                            {
                                                // first ray comes from cam
                                                // Let ray bounce around and determine the color
                                                auto const raymarch = MarchRay(Line{Camera::Origin, cameraTransformation.Transform(x, y)});

                                                // apply color filters on the emmision spectrum
                                                auto const pixelcolor = raymarch.Emissions.MultiplyElementwise(raymarch.ColorFilters);

                                                // paint pixel with object color into *image space*
                                                std::copy(pixelcolor.Data.begin(), pixelcolor.Data.end(), output.atPixel(x, y));
                                            }
                                        } },
                                     argFromY, argToY);
            argFromY += yStep;
        }

        // wait for completion
        for (auto &thread : sidethreads)
            thread.join();
    }

    RayIntersection_t FindClosestIntersection(Line const &ray, Shapes::Shape const *ignoreShape)
    {
        std::array<RayIntersection_t, Scene::Objects.size()> hits = {};
        for (unsigned int i = 0; i < Scene::Objects.size(); i++)
        {
            Shapes::Shape const *shape = Scene::Objects[i];

            if (shape == ignoreShape)
                continue;

            auto hitevent = shape->CheckHit(ray);
            if (!hitevent)
                continue;

            hits[i] = RayIntersection_t{shape, hitevent.value()};
        }

        // find element with shortest distance
        double shortestDistance = INFINITY;
        decltype(hits)::iterator nearest = hits.end();
        for (auto it = hits.begin(); it != hits.end(); it++)
        {
            if (!it->Shape)
                continue;

            if (it->Hitevent.DistanceToSurface >= shortestDistance)
                continue;

            shortestDistance = it->Hitevent.DistanceToSurface;
            nearest = it;
        }

        if (nearest == hits.end())
        {
            // nothing hit, bad?
            DEBUG_WARN("Ray did not hit anything!");
            return RayIntersection_t{};
        }

        return *nearest;
    }

    Raytracer::RayMarchResult Raytracer::MarchRay(Line const &ray)
    {
        struct stackItem_t
        {
            Shapes::Shape const *SourceShape;
            Line SourceRay;
            Line ReflectedRay;
            Vec3d SurfaceNormal;
            Vec3d InheritedColorTransferFunction = {1, 1, 1};
            double InheritedWeight;
            double ApparentDiffusionFactor;
            unsigned char Num_Children;
        };
        std::array<stackItem_t, NUM_RAY_BOUNCES + 1> rayGenerationStack{};
        auto const y = sizeof(rayGenerationStack);
        static_assert(y < 1024ULL * 1024ULL, "Stack size critical");

        rayGenerationStack[0].SourceRay = ray;

        RayMarchResult accumulator{};

        // build tree from bottom up
        size_t recursionDepth = 0;
        while (true)
        {
            stackItem_t &currentStackItem = rayGenerationStack[recursionDepth];
            if (currentStackItem.Num_Children == 0)
            {
                auto const closestIntersection = FindClosestIntersection(currentStackItem.SourceRay, currentStackItem.SourceShape);
                if (!closestIntersection.Shape)
                {
                    // nothing hit, bad?
                    DEBUG_WARN("Ray did not hit anything!");

                    // unwind stack
                    if (recursionDepth-- == 0)
                        break;
                }

                Materials::Material const &material = closestIntersection.Shape->Material;

                // check if light source was hit
                if (material.IsLightsource)
                {
                    accumulator.Emissions = accumulator.Emissions + material.Emission * currentStackItem.InheritedWeight;

                    // unwind stack
                    if (recursionDepth-- == 0)
                        break;
                }

                if (recursionDepth == NUM_RAY_BOUNCES)
                {
                    // did not hit lightsource, no contribution
                }

                // store state
                Shapes::HitEvent const &hitevent = closestIntersection.Hitevent;
                currentStackItem.ReflectedRay = hitevent.ReflectedRay;
                currentStackItem.SurfaceNormal = hitevent.SurfaceNormal;
                currentStackItem.InheritedColorTransferFunction = currentStackItem.InheritedColorTransferFunction.MultiplyElementwise(material.ColorFilter);

                // total reflection: Material then acts like a perfect mirror
                double const angleOfIncidence = abs(hitevent.ReflectedRay.Direction.AngleTo(hitevent.SurfaceNormal));
                // lerp between diffusion factor and 0 between the range critical angle .. 90°
                currentStackItem.ApparentDiffusionFactor = angleOfIncidence < material.CriticalAngle ? material.DiffusionFactor
                                                                                                     : std::lerp(material.DiffusionFactor, 0.0, (angleOfIncidence - material.CriticalAngle) / (Deg2Rad(90) - material.CriticalAngle));
                DEBUG_BREAKIF(currentStackItem.ApparentDiffusionFactor < material.DiffusionFactor);

                DEBUG_ASSERT(currentStackItem.ApparentDiffusionFactor > 0 && currentStackItem.ApparentDiffusionFactor <= material.DiffusionFactor, "Bad diffusion fadeout");
                // add mirror-like ray
                double const weight = 1.0 - currentStackItem.ApparentDiffusionFactor;

                // push to stack
                currentStackItem.Num_Children = 1;
                rayGenerationStack[recursionDepth + 1].SourceRay = hitevent.ReflectedRay;
                rayGenerationStack[recursionDepth + 1].InheritedWeight = weight;
                rayGenerationStack[recursionDepth + 1].SourceShape = closestIntersection.Shape;

                // go deeper
                if (recursionDepth < NUM_RAY_BOUNCES)
                {
                    recursionDepth += 1;
                    continue;
                }
            }

            // perfect mirror will only spawn single ray
            if ((currentStackItem.ApparentDiffusionFactor > 0) && currentStackItem.Num_Children < NUM_DIFFUSE_RAYS)
            {
                // Start from surface normal
                Line probingRay{currentStackItem.SourceRay.Origin, currentStackItem.SurfaceNormal};

                // rotate away from surface normal in the plane (surface normal) x (reflection)
                probingRay.Direction = probingRay.Direction.RotateAboutPlane(currentStackItem.SurfaceNormal, currentStackItem.SourceRay.Direction, RandDouble() * Deg2Rad(60));

                // start rotating about the normal in appropriate steps
                probingRay.Direction = probingRay.Direction.RotateAboutAxis(currentStackItem.SurfaceNormal, RandDouble() * Deg2Rad(360));

                DEBUG_ASSERT(AlmostSame(probingRay.Direction.GetNorm(), 1.0), "Rotation is bad for vector");

                // Store in tree
                // both have norm = 1! So this weights parallel lines to 1 and perpendicular to 0
                double const weight = abs(currentStackItem.SourceRay.Direction * probingRay.Direction);

                // push to stack
                currentStackItem.Num_Children++;
                rayGenerationStack[recursionDepth + 1].SourceRay = currentStackItem.ReflectedRay;
                rayGenerationStack[recursionDepth + 1].InheritedWeight = weight;

                // go deeper
                if (recursionDepth < NUM_RAY_BOUNCES)
                {
                    recursionDepth += 1;
                    continue;
                }
            }

            // walk tree up
            recursionDepth += 1;
            continue;
        }

        return accumulator;
    }

    // Raytracer::RayMarchResult Raytracer::MarchRay(Line const &ray, Shapes::Shape const *disabledCollision, unsigned int const recursionDepth)
    // {
    //     // see if ray intersects any object

    //     std::array<RayIntersection_t, Scene::Objects.size()> hits = {};
    //     for (unsigned int i = 0; i < Scene::Objects.size(); i++)
    //     {
    //         Shapes::Shape const *shape = Scene::Objects[i];

    //         if (shape == disabledCollision)
    //             continue;

    //         auto hitevent = shape->CheckHit(ray);
    //         if (!hitevent)
    //             continue;

    //         hits[i] = RayIntersection_t{shape, hitevent.value()};
    //     }

    //     // find element with shortest distance
    //     double shortestDistance = INFINITY;
    //     decltype(hits)::iterator nearest = hits.end();
    //     for (auto it = hits.begin(); it != hits.end(); it++)
    //     {
    //         if (!it->Shape)
    //             continue;

    //         if (it->Hitevent.DistanceToSurface >= shortestDistance)
    //             continue;

    //         shortestDistance = it->Hitevent.DistanceToSurface;
    //         nearest = it;
    //     }

    //     if (nearest == hits.end())
    //     {
    //         // nothing hit, bad?
    //         DEBUG_WARN("Ray did not hit anything!");
    //         return RayMarchResult{};
    //     }

    //     Materials::Material const &material = nearest->Shape->Material;

    //     // check if light source was hit
    //     if (material.IsLightsource)
    //     {
    //         return RayMarchResult{.ColorFilters = {1, 1, 1}, .Emissions = material.Emission};
    //     }

    //     Vec3d colorFilterAccumulator;
    //     Vec3d emissionAccumulator;
    //     // Stop recursion at some depth
    //     if (recursionDepth == NUM_RAY_BOUNCES)
    //     {
    //         colorFilterAccumulator = Vec3d{1, 1, 1};
    //         emissionAccumulator = Vec3d{0, 0, 0};
    //     }
    //     else
    //     {
    //         struct rayProbe_t
    //         {
    //             RayMarchResult ProbedResult;
    //             double LambertianFactor;
    //         };
    //         std::array<rayProbe_t, NUM_DIFFUSE_RAYS> rayProbes = {};

    //         // total reflection: Material then acts like a perfect mirror
    //         double const angleOfIncidence = abs(nearest->Hitevent.ReflectedRay.Direction.AngleTo(nearest->Hitevent.SurfaceNormal));
    //         // lerp between diffusion factor and 0 between the range critical angle .. 90°
    //         double const apparentDiffusionFactor = angleOfIncidence < material.CriticalAngle ? material.DiffusionFactor
    //                                                                                          : std::lerp(material.DiffusionFactor, 0.0, (angleOfIncidence - material.CriticalAngle) / (Deg2Rad(90) - material.CriticalAngle));

    //         DEBUG_BREAKIF(apparentDiffusionFactor < material.DiffusionFactor);

    //         // perfect mirror will only spawn single ray
    //         rayProbes[0] = rayProbe_t{MarchRay(nearest->Hitevent.ReflectedRay, nearest->Shape, recursionDepth + 1), (1.0 - apparentDiffusionFactor)};

    //         if (apparentDiffusionFactor > 0)
    //         {
    //             // Start from surface normal
    //             Line probingRay{nearest->Hitevent.ReflectedRay.Origin, nearest->Hitevent.SurfaceNormal};

    //             // spawn random rays, lesser and lesser the more depth
    //             for (size_t j = 1; j < (NUM_DIFFUSE_RAYS - recursionDepth) - 1; j++)
    //             {
    //                 // rotate away from surface normal in the plane (surface normal) x (reflection)
    //                 probingRay.Direction = probingRay.Direction.RotateAboutPlane(nearest->Hitevent.SurfaceNormal, nearest->Hitevent.ReflectedRay.Direction, RandDouble() * Deg2Rad(60));

    //                 // start rotating about the normal in appropriate steps
    //                 probingRay.Direction = probingRay.Direction.RotateAboutAxis(nearest->Hitevent.SurfaceNormal, RandDouble() * Deg2Rad(360));

    //                 DEBUG_ASSERT(AlmostSame(probingRay.Direction.GetNorm(), 1.0), "Rotation is bad for vector");

    //                 rayProbes[j] = rayProbe_t{
    //                     // recursively march
    //                     MarchRay(probingRay, nearest->Shape, recursionDepth + 1),
    //                     // both have norm = 1! So this weights parallel lines to 1 and perpendicular to 0
    //                     abs(nearest->Hitevent.ReflectedRay.Direction * probingRay.Direction)};
    //             }
    //         }

    //         // calc weighted average over the lambertian factor
    //         colorFilterAccumulator = Vec3d{0, 0, 0};
    //         emissionAccumulator = Vec3d{0, 0, 0};
    //         double const weightSum = std::accumulate(rayProbes.begin(), rayProbes.end(), 0.0, [](double const &last, rayProbe_t const &elem)
    //                                                  { return last + elem.LambertianFactor; });
    //         for (size_t j = 0; j < NUM_DIFFUSE_RAYS; j++)
    //         {
    //             colorFilterAccumulator = colorFilterAccumulator + (rayProbes[j].LambertianFactor / weightSum) * Vec3d{rayProbes[j].ProbedResult.ColorFilters};
    //             emissionAccumulator = emissionAccumulator + (rayProbes[j].LambertianFactor / weightSum) * Vec3d{rayProbes[j].ProbedResult.Emissions};
    //         }
    //     }

    //     // merge with material of hit shape
    //     RayMarchResult const result{.ColorFilters = colorFilterAccumulator.MultiplyElementwise(material.ColorFilter), .Emissions = emissionAccumulator + material.Emission};
    //     return result;
    // }

    double Raytracer::RandDouble()
    {
        return std::uniform_real_distribution<double>{}(RngEngine);
    }
}