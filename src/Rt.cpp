#include "Rt.h"

#include <random>
#include <algorithm>

namespace Rt
{
    class
    {
    public:
        double Next()
        {
            return Distribution(Engine);
        }

    private:
        std::uniform_real_distribution<double> Distribution{0, 1};
        std::default_random_engine Engine{};
    } Rng;

    Color_t MarchRay(Line ray, unsigned int const recursionDepth = 0)
    {
        ColorD_t colorFilters = {1, 1, 1};
        Color_t lightsource = {0, 0, 0};

        for (size_t iteration = 0; iteration < (BOUNCE_ITERATIONS >> recursionDepth); iteration++)
        {
            // see if ray intersects any object
            std::vector<std::pair<Shapes::Shape *, Shapes::HitEvent>> hits;
            for (unsigned int i = 0; i < Scene::Objects.size(); i++)
            {
                Shapes::Shape *shape = Scene::Objects[i];
                auto hitevent = shape->CheckHit(ray);
                if (!hitevent)
                    continue;

                hits.push_back(std::make_pair(shape, hitevent.value()));
            }

            if (hits.size() == 0)
                // nothing hit
                break;

            // find element with shortest distance
            double shortestDistance = INFINITY;
            decltype(hits)::iterator nearest;
            for (auto it = hits.begin(); it != hits.end(); it++)
            {
                if (it->second.DistanceToSurface >= shortestDistance)
                    continue;

                shortestDistance = it->second.DistanceToSurface;
                nearest = it;
            }

            // check if light was hit
            Shapes::MaterialInfo const &material = nearest->first->Material;
            if (material.IsLightsource)
            {
                lightsource = material.Emission;
                break;
            }

            if (material.DiffusionFactor > 0)
            {
                struct RayProbe
                {
                    Color_t ProbedColor;
                    double Weight;
                };
                // spawn random rays
                std::array<RayProbe, DIFFUSE_RAYS> rayProbes;
                for (size_t j = 0; j < (DIFFUSE_RAYS >> recursionDepth); j++)
                {
                    // TODO: better way for random rays
                    Line randomRay{ray};
                    randomRay.Direction = (randomRay.Direction + Vec3d{Rng.Next(), Rng.Next(), Rng.Next()}).ToNormalized();

                    RayProbe const result{
                        // RECURSION!
                        .ProbedColor = MarchRay(randomRay, recursionDepth + 1),
                        // angle formula kinda optimized since both have norm = 1
                        .Weight = Deg2Rad(90) - acos(ray.Direction * randomRay.Direction)};

                    rayProbes[j] = result;
                }

                // calc weighted average over the inverse angle
                double const weightSum = std::accumulate(rayProbes.begin(), rayProbes.end(), 0, [](int & last, auto const &elem)
                                                         { return last + elem.Weight; });
                ColorD_t accumulator{};
                for (size_t j = 0; j < DIFFUSE_RAYS; j++)
                {
                    rayProbes[j].Weight /= weightSum;
                    accumulator = accumulator + (rayProbes[j].Weight / weightSum) * Vec3d{rayProbes[j].ProbedColor};
                }

                Color_t const diffuseRayResult = (accumulator * (1 / weightSum)).Cast<unsigned char>();
            }
            else
            {
                ray = nearest->second.ReflectedRay;
            }

            // apply color filter
            colorFilters = colorFilters.MultiplyElementwise(material.TransferFunction);

            // apply lambertian law
            double const lambertianFactor = nearest->second.SurfaceNormal * ray.Direction;

            colorFilters = colorFilters * lambertianFactor;
        }

        // calculate color filters for each hit
        Color_t pixelcolor;
        for (unsigned int i = 0; i < 3; i++)
            pixelcolor[i] = (unsigned char)((double)lightsource[i] * colorFilters.Data[i]);

        return pixelcolor;
    }

    Bitmap::Bitmap RT()
    {
        // seed for rng
        std::srand(42);

        Bitmap::Bitmap bitmap = {0};

        // Camera rays
        Transformation::Map2Sphere const cameraTransformation{Bitmap::BITMAP_WIDTH, Bitmap::BITMAP_HEIGHT, Camera::FOV};
        for (size_t y = 0; y < Bitmap::BITMAP_HEIGHT; y++)
        {
            for (size_t x = 0; x < Bitmap::BITMAP_WIDTH; x++)
            {
                // first ray comes from cam
                Line ray = {Camera::Origin, cameraTransformation.Transform(x, y)};

                // Let ray bounce around and determine the color
                auto const pixelcolor = MarchRay(ray);

                // paint pixel with object color
                std::copy(pixelcolor.begin(), pixelcolor.end(), bitmap.at(x, y));
            }
        }

        return bitmap;
    }
}