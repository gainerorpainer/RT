#pragma once

#include <array>

namespace Bitmap
{
    template <typename T, size_t WIDTH, size_t HEIGHT, size_t COLORS>
    class _bitmap_ofsize_t
    {
    public:
        std::array<T, WIDTH * HEIGHT * COLORS> Pixels;

        constexpr size_t Width() const noexcept { return WIDTH; };
        constexpr size_t Height() const noexcept { return HEIGHT; };
        constexpr size_t Colors() const noexcept { return COLORS; };

        T const &at(unsigned int x, unsigned int y, unsigned int color) const
        {
            return Pixels[(WIDTH * COLORS) * y + COLORS * x + color];
        }

        T &at(unsigned int x, unsigned int y, unsigned int color)
        {
            return Pixels[(WIDTH * COLORS) * y + COLORS * x + color];
        }

        /// @brief References a pixel at physical location and maps it to image location
        /// @param x X in physical coords
        /// @param y y in physical coords
        /// @return Color of that pixel in image space
        constexpr T *atPixel(unsigned int x, unsigned int y)
        {
            return Pixels.begin() + ((WIDTH * COLORS) * (HEIGHT - 1 - y) + COLORS * x);
        }

        template <typename T2>
        _bitmap_ofsize_t<T2, WIDTH, HEIGHT, COLORS> Cast() const noexcept
        {
            _bitmap_ofsize_t<T2, WIDTH, HEIGHT, COLORS> result{};
            std::copy(Pixels.begin(), Pixels.end(), result.Pixels.begin());
            return result;
        }
    };

    constexpr unsigned int STD_BITMAP_WIDTH = 800;
    constexpr unsigned int STD_BITMAP_HEIGHT = 600;
    constexpr unsigned int STD_COLOR_COUNT = 3;

    constexpr unsigned int STD_BUFFERSIZE = STD_BITMAP_WIDTH * STD_BITMAP_HEIGHT * STD_COLOR_COUNT;

    using Bitmap = _bitmap_ofsize_t<unsigned char, STD_BITMAP_WIDTH, STD_BITMAP_HEIGHT, STD_COLOR_COUNT>;
    using BitmapD = _bitmap_ofsize_t<double, STD_BITMAP_WIDTH, STD_BITMAP_HEIGHT, STD_COLOR_COUNT>;
}