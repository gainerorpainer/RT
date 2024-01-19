#pragma once

namespace Bitmap
{
    constexpr unsigned int BITMAP_WIDTH = 100;
    constexpr unsigned int BITMAP_HEIGHT = 100;

    struct Bitmap
    {
        unsigned char Pixels[BITMAP_WIDTH * BITMAP_HEIGHT][3] = {0};

        unsigned char *at(unsigned int x, unsigned int y)
        {
            return Pixels[BITMAP_WIDTH * y + x];
        }
    };
}