#include "main.h"

#include <iostream>
#include <algorithm>
#include <vector>

#include "Rt.h"
#include "ImgFile.h"

// exe entry point
int main()
{
    Bitmap::Bitmap const bitmap = Rt::RT();

    ImgFile::writeNetPbm("Render\\output.ppm", Bitmap::BITMAP_WIDTH, Bitmap::BITMAP_HEIGHT, bitmap.Pixels);

    std::cout << "Done" << std::endl;

    return 0;
}