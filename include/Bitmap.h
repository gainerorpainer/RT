#pragma once

#include <array>

namespace Bitmap
{
    constexpr unsigned int BITMAP_WIDTH = 800;
    constexpr unsigned int BITMAP_HEIGHT = 600;
    constexpr unsigned int COLOR_COUNT = 3;

    constexpr unsigned int BUFFERSIZE = BITMAP_WIDTH * BITMAP_HEIGHT * COLOR_COUNT;

    template <typename T>
    class _bitmap_t
    {
    public:
        std::array<T, BUFFERSIZE> Pixels = {0};

        T const &at(unsigned int x, unsigned int y, unsigned int color) const
        {
            return Pixels[(BITMAP_WIDTH * COLOR_COUNT) * y + COLOR_COUNT * x + color];
        }

        T &at(unsigned int x, unsigned int y, unsigned int color)
        {
            return Pixels[(BITMAP_WIDTH * COLOR_COUNT) * y + COLOR_COUNT * x + color];
        }

        /// @brief References a pixel at physical location and maps it to image location
        /// @param x X in physical coords
        /// @param y y in physical coords
        /// @return Color of that pixel in image space
        constexpr T *atPixel(unsigned int x, unsigned int y)
        {
            return Pixels.begin() + ((BITMAP_WIDTH * COLOR_COUNT) * (Bitmap::BITMAP_HEIGHT - 1 - y) + COLOR_COUNT * x);
        }

        template <typename T2>
        _bitmap_t<T2> Cast() const
        {
            _bitmap_t<T2> result{};
            std::copy(Pixels.begin(), Pixels.end(), result.Pixels.begin());
            return result;
        }
    };

    using Bitmap = _bitmap_t<unsigned char>;
    using BitmapD = _bitmap_t<double>;
}