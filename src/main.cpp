#include <iostream>
#include <algorithm>
#include <vector>

#include "Bitmap.h"
#include "Camera.h"
#include "Scene.h"

#include "Transformation.h"

#include "ImgFile.h"

Bitmap::Bitmap RT()
{
    Bitmap::Bitmap bitmap = {0};

    // Camera rays
    Transformation::Map2Sphere const cameraTransformation{Bitmap::BITMAP_WIDTH, Bitmap::BITMAP_HEIGHT, Camera::FOV};
    for (size_t y = 0; y < Bitmap::BITMAP_HEIGHT; y++)
    {
        for (size_t x = 0; x < Bitmap::BITMAP_WIDTH; x++)
        {
            for (size_t iteration = 0; iteration < 5; iteration++)
            {
                /* code */
            }
            
            // calc ray vector by mapping 2d to sphere coords
            Vec3d const rayDirection = cameraTransformation.Transform(x, y);

            // see if ray intersects any object
            std::vector<std::pair<Shapes::Shape *, Shapes::HitEvent>> hits;
            for (unsigned int i = 0; i < Scene::Objects.size(); i++)
            {
                Shapes::Shape *shape = Scene::Objects[i];
                auto hitevent = shape->CheckHit(Line{Camera::Origin, rayDirection});
                if (!hitevent)
                    continue;

                hits.push_back(std::make_pair(shape, hitevent.value()));
            }

            if (hits.size() == 0)
                continue;

            // find element with shortest distance
            std::pair<Shapes::Shape *, double> nearest = std::make_pair(hits.front().first, hits.front().second.Distance);
            for (auto it = std::next(hits.begin()); it != hits.end(); it++)
            {
                if (it->second.Distance >= nearest.second)
                    continue;
                nearest = std::make_pair(it->first, it->second.Distance);
            }

            // paint pixel with object color
            std::copy(nearest.first->Emission.begin(), nearest.first->Emission.end(), bitmap.at(x, y));
        }
    }

    return bitmap;
}

int main()
{
    Bitmap::Bitmap const bitmap = RT();

    ImgFile::writeNetPbm("Render\\output.ppm", Bitmap::BITMAP_WIDTH, Bitmap::BITMAP_HEIGHT, bitmap.Pixels);

    std::cout << "Done" << std::endl;

    return 0;
}
