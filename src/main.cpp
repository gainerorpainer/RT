#include "main.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <execution>
#include <thread>
#include <memory>

#include "Rt.h"
#include "ImgFile.h"

#ifdef NDEBUG
// optimize through parallel threads for denoising
constexpr auto EXECUTION_POLICY = std::execution::par;
#else
constexpr auto EXECUTION_POLICY = std::execution::seq;
#endif

// exe entry point
int main()
{
    const unsigned int workerCount = 1; // std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() : 1;

    std::vector<std::unique_ptr<Bitmap::BitmapD>> workerResults;
    workerResults.resize(workerCount);

    std::cout << "Raytracing..." << std::endl;


    std::for_each(EXECUTION_POLICY, workerResults.begin(), workerResults.end(), [](std::unique_ptr<Bitmap::BitmapD> &target)
                  {
                      // heap alloc bitmap
                      std::unique_ptr<Bitmap::BitmapD> bitmap{new Bitmap::BitmapD{}};
                      auto raytracer = Rt::Raytracer{};
                      raytracer.RunBitmap(*bitmap);

                      // transfer ownership
                      target = std::move(bitmap); });

    std::cout << "Averaging..." << std::endl;

    // average all results
    std::unique_ptr<Bitmap::BitmapD> resultBuffer{new Bitmap::BitmapD{}};
    for (auto const &piece : workerResults)
    {
        for (size_t y = 0; y < Bitmap::BITMAP_HEIGHT; y++)
        {
            for (size_t x = 0; x < Bitmap::BITMAP_WIDTH; x++)
            {
                for (size_t i = 0; i < 3; i++)
                {
                    resultBuffer->at(x, y, i) += piece->at(x, y, i) / (double)workerCount;
                }
            }
        }
    }

    std::cout << "Writing to file..." << std::endl;

    ImgFile::writeNetPbm("Render\\output.ppm", *resultBuffer);

    std::cout << "Done" << std::endl;

    return 0;
}