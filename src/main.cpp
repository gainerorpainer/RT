#include <iostream>

#include "Types.h"

#include "Bitmap.h"
#include "Camera.h"
#include "Scene.h"

#include "Transformation.h"

#include "ImgFile.h"

using namespace Types;

int main()
{
    std::cout << "Hello World" << std::endl;

    Bitmap::Bitmap bitmap = {0};

    // Camera rays
    Transformation::Map2Sphere const cameraTransformation{Bitmap::BITMAP_WIDTH, Bitmap::BITMAP_HEIGHT, Camera::FOV};
    for (size_t x = 0; x < Bitmap::BITMAP_WIDTH; x++)
    {
        for (size_t y = 0; y < Bitmap::BITMAP_HEIGHT; y++)
        {
            // calc ray vector by mapping 2d to sphere coords
            Vec3d_t const rayDirection = cameraTransformation.Transform(x, y);

            // see if ray intersects any object
            for (auto const &object : Scene::Objects)
            {
                if(!object.IsIntersecting(std::make_tuple(Camera::Origin, rayDirection)))
                    continue;

                // paint pixel with object color
                std::copy(object.Color.begin(), object.Color.end(), bitmap.at(x, y));
            }
        }
    }

    // // draw random stuff
    // for (size_t i = 0; i < std::min(Bitmap::BITMAP_HEIGHT, Bitmap::BITMAP_WIDTH); i++)
    //     for (size_t j = 0; j < 3; j++)
    //         bitmap.at(i, i)[j] = 100;

    ImgFile::writeNetPbm("render\\test.ppm", Bitmap::BITMAP_WIDTH, Bitmap::BITMAP_HEIGHT, bitmap.Pixels);

    return 0;
}