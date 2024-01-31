#include "ImgFile.h"

#include <stdio.h>
#include <stdlib.h>

namespace ImgFile
{
    void writeNetPbm(std::string targetFile, unsigned int width, unsigned int height, unsigned char const rgbData[][3])
    {
        FILE *const imageFile = fopen(targetFile.c_str(), "wb");
        if (imageFile == NULL)
        {
            perror("ERROR: Cannot open output file");
            exit(EXIT_FAILURE);
        }

        fprintf(imageFile, "P6\n");                   // filetype
        fprintf(imageFile, "%u %u\n", width, height); // dimensions
        fprintf(imageFile, "255\n");                  // pixeldepth

        // upside down
        for (size_t y = height; y != (size_t)-1; y--)
        {
            for (size_t x = 0; x < width; x++)
            {
                for (size_t i = 0; i < 3; i++)
                    fputc(rgbData[y * width + x][i], imageFile);
            }
        }

        fclose(imageFile);
    }
} // namespace ImgFile
