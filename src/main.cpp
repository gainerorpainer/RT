#include "main.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <thread>
#include <memory>

#include "Rt.h"
#include "ImgFile.h"

#ifdef NDEBUG
// optimize through parallel threads
constexpr bool USE_PARALLEL = true;
#else
constexpr bool USE_PARALLEL = false;
#endif

// exe entry point
int main()
{
    std::cout << "Raytracing..." << std::endl;
    std::unique_ptr<Bitmap::BitmapD> resultBuffer{new Bitmap::BitmapD{}};
    if (!USE_PARALLEL)
    {
        auto raytracer = Rt::Raytracer{};
        raytracer.RunBitmap(*resultBuffer);
    }
    else
    {
        const unsigned int workerCount = std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() : 1;
        std::vector<std::shared_ptr<Bitmap::BitmapD>> workerResults;
        std::vector<std::thread> sidethreads;
        for (size_t i = 0; i < workerCount; i++)
        {
            // heap alloc bitmap
            workerResults.push_back(std::make_shared<Bitmap::BitmapD>());

            // run as sidethread
            sidethreads.emplace_back([](std::shared_ptr<Bitmap::BitmapD> bitmap)
                                     {
                                    auto raytracer = Rt::Raytracer{};
                                    raytracer.RunBitmap(*bitmap); },
                                     workerResults.back());
        }

        for (auto &thread : sidethreads)
            thread.join();

        std::cout << "Averaging..." << std::endl;

        // average all results
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
    }

    std::cout << "Writing to file..." << std::endl;

    ImgFile::writeNetPbm("Render\\output.ppm", *resultBuffer);

    std::cout << "Done" << std::endl;

    return 0;
}