#include "main.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <thread>
#include <memory>

#include "Rt.h"
#include "ImgFile.h"
#include "TESTS.h"

#ifdef NDEBUG
// optimize through parallel threads
constexpr bool USE_PARALLEL = true;
// skip tests
constexpr bool DO_TESTS = false;
#else
constexpr bool USE_PARALLEL = false;
constexpr bool DO_TESTS = false;
#endif

void Sidethread(std::shared_ptr<Bitmap::BitmapD> bitmap)
{
    unsigned int const rngSeed = std::hash<std::thread::id>{}(std::this_thread::get_id());
    auto raytracer = Rt::Raytracer{rngSeed};
    raytracer.RunBitmap(*bitmap);
}

// exe entry point
int main()
{
    if (DO_TESTS)
    {
        std::cout << "Tests..." << std::endl;
        Tests::RunTests();
    }

    std::cout << "Raytracing..." << std::endl;
    std::unique_ptr<Bitmap::BitmapD> resultBuffer{new Bitmap::BitmapD{}};
    if (!USE_PARALLEL)
    {
        auto raytracer = Rt::Raytracer{};
        raytracer.RunBitmap(*resultBuffer);
    }
    else
    {
        // sidethread handling
        const unsigned int workerCount = std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() : 1;
        std::vector<std::shared_ptr<Bitmap::BitmapD>> workerResults;
        std::vector<std::thread> sidethreads;
        for (size_t i = 0; i < workerCount; i++)
        {
            // heap alloc bitmap
            workerResults.push_back(std::make_shared<Bitmap::BitmapD>());

            // run as sidethread
            sidethreads.emplace_back(Sidethread, workerResults.back());
        }
        for (auto &thread : sidethreads)
            thread.join();

        // average all results
        std::cout << "Averaging..." << std::endl;
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