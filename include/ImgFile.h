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

    void writeNetPbm(std::string targetFile, Bitmap::BitmapD const &bitmap)
    {
        std::ofstream outfile{targetFile, std::ios::out | std::ios::binary};
        if (!outfile.good())
            DEBUG_CRASH("Cannot open output file");

        // write header
        std::string const header = FILETYPE_MAGICNUMBER + NEWLINE + (std::to_string(Bitmap::STD_BITMAP_WIDTH) + " " + std::to_string(Bitmap::STD_BITMAP_HEIGHT)) + NEWLINE + PIXELDEPTH + NEWLINE;
        outfile.write((char const *)header.c_str(), header.length());

        // cast to output format & write
        Bitmap::Bitmap targetFormat;
        std::copy(bitmap.Pixels.begin(), bitmap.Pixels.end(), targetFormat.Pixels.begin());
        outfile.write((char const *)targetFormat.Pixels.begin(), targetFormat.Pixels.size());

        outfile.close();
    }
}
