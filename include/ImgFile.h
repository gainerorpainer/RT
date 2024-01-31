#pragma once

#include <string>

namespace ImgFile
{
    void writeNetPbm(std::string targetFile, unsigned int width, unsigned int height, unsigned char const rgbData[][3]);
} // namespace Bitmap
