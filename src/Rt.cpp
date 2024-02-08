#include "Rt.h"

namespace Rt
{
    Color_t MarchRay(Line ray)
    {
        ColorD_t colorFilters = {1, 1, 1};
        Color_t lightsource = {0, 0, 0};

        for (size_t iteration = 0; iteration < ITERATIONS; iteration++)
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

            // apply material and new ray(s)
            Shapes::MaterialInfo const &material = nearest->first->Material;

            ray = nearest->second.ReflectedRay;

            if (material.IsLightsource)
            {
                lightsource = material.Emission;
                break;
            }
            else
            {
                // apply color filter
                colorFilters = colorFilters.MultiplyElementwise(material.TransferFunction);

                // apply lambertian law
                double const lambertianFactor = nearest->second.SurfaceNormal * ray.Direction;

                colorFilters = colorFilters * lambertianFactor;
            }
        }

        // calculate color filters for each hit
        Color_t pixelcolor;
        for (unsigned int i = 0; i < 3; i++)
            pixelcolor[i] = (unsigned char)((double)lightsource[i] * colorFilters.Data[i]);

        return pixelcolor;
    }

    Bitmap::Bitmap RT()
    {
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