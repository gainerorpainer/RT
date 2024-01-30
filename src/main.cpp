#include <iostream>
#include <algorithm>

#include "Bitmap.h"
#include "Camera.h"
#include "Scene.h"

#include "Transformation.h"

#include "ImgFile.h"

int main()
{
    std::cout << "Hello World" << std::endl;

    Bitmap::Bitmap bitmap = {0};

    // apply z-index w/ respect to camera
    auto asSorted = [](auto const & array, auto isFirstFunc) { 
        auto copy{array};
        std::sort(copy.begin(), copy.end(), isFirstFunc);
    };
    auto getDistanceToCam = [](Shapes::Shape const & shape) 
    {
        return shape.Center;
    };
    auto isCloser = [](Shapes::Shape const & first, Shapes::Shape const & second)
    {
        return 
    };
    std::array<Shapes::Shape *, Scene::Objects.size()> const sortedObjects = asSorted(Scene::Objects, nullptr);

    // Camera rays
    Transformation::Map2Sphere const cameraTransformation{Bitmap::BITMAP_WIDTH, Bitmap::BITMAP_HEIGHT, Camera::FOV};
    for (size_t x = 0; x < Bitmap::BITMAP_WIDTH; x++)
    {
        for (size_t y = 0; y < Bitmap::BITMAP_HEIGHT; y++)
        {
            // calc ray vector by mapping 2d to sphere coords
             Vec3d const rayDirection = cameraTransformation.Transform(x, y);

            // see if ray intersects any object
            std::array<std::optional<Shapes::HitEvent>, Scene::Objects.size()> hits;
            for (Shapes::Shape const *const object : Scene::Objects)
            {
                auto const hitevent = object->CheckHit(Line{Camera::Origin, rayDirection});
                if (!hitevent)
                    continue;

                // paint pixel with object color
                std::copy(object->Emission.begin(), object->Emission.end(), bitmap.at(x, y));
            }
        }
    }

    ImgFile::writeNetPbm("Render\\output.ppm", Bitmap::BITMAP_WIDTH, Bitmap::BITMAP_HEIGHT, bitmap.Pixels);

    return 0;
}