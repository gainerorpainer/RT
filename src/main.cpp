#include <iostream>

#include "Bitmap.h"
#include "ImgFile.h"

int main()
{
    std::cout << "Hello World" << std::endl;

    Bitmap::Bitmap bitmap = {0};

    // draw random stuff
    for (size_t i = 0; i < std::min(Bitmap::BITMAP_HEIGHT, Bitmap::BITMAP_WIDTH); i++)
        for (size_t j = 0; j < 3; j++)
            bitmap.at(i, i)[j] = 100;

    ImgFile::writeNetPbm("render\\test.ppm", Bitmap::BITMAP_WIDTH, Bitmap::BITMAP_HEIGHT, bitmap.Pixels);

    return 0;
}