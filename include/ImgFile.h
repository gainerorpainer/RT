#pragma once

#include <string>
#include <fstream>
#include <stdio.h>

#include "Bitmap.h"
#include "Debug.h"

namespace ImgFile
{
    constexpr std::string NEWLINE = "\n";
    constexpr std::string FILETYPE_MAGICNUMBER = "P6";
    constexpr std::string PIXELDEPTH = "255";

    template <typename T>
    void writeNetPbm(std::string targetFile, Bitmap::_bitmap_t<T> const &bitmap)
    {
        std::ofstream outfile{targetFile, std::ios::out | std::ios::binary};
        if (!outfile.good())
            DEBUG_CRASH("Cannot open output file");

        // write header
        std::string const header = FILETYPE_MAGICNUMBER + NEWLINE + (std::to_string(Bitmap::BITMAP_WIDTH) + " " + std::to_string(Bitmap::BITMAP_HEIGHT)) + NEWLINE + PIXELDEPTH + NEWLINE;
        outfile.write((char const *)header.c_str(), header.length());

        // cast to output format & write
        std::unique_ptr<Bitmap::Bitmap> targetFormat{new Bitmap::Bitmap};
        for (size_t y = 0; y < Bitmap::BITMAP_HEIGHT; y++)
            for (size_t x = 0; x < Bitmap::BITMAP_WIDTH; x++)
                for (size_t i = 0; i < 3; i++)
                    targetFormat->at(x, y, i) += (unsigned char)bitmap.at(x, y, i);

        outfile.write((char const *)targetFormat->Pixels.begin(), targetFormat->Pixels.size());

        outfile.close();
    }
}
