#pragma once

#include "Primitives.h"

namespace Materials
{
    using namespace Primitives;

    struct Material
    {
        /// @brief Is this a light source -> ends the ray's path
        bool IsLightsource = false;

        /// @brief Emission spectrum, should be just in conjunction with IsLightsource
        ColorD_t Emission = {0, 0, 0};

        /// @brief Color Filtering, basically a multiplication with a emission spectrum
        ColorD_t ColorFilter = {1, 1, 1};

        /// @brief How diffuse the material is, 0 is a perfect mirror, 1 is a fuzzy material
        double DiffusionFactor = 0;

        /// @brief Indicent angle at which (lower than this value) total reflection occurs
        double CriticalAngle = Deg2Rad(0);

        inline Material &MakeEmissive(Color_t emissionSpectrum)
        {
            Emission = emissionSpectrum;
            IsLightsource = true;
            return *this;
        }

        inline Material &MakeAbsorbing(Color_t visibleColor)
        {
            ColorFilter = ColorD_t{(double)visibleColor[0] / 255.0, (double)visibleColor[1] / 255.0, (double)visibleColor[2] / 255.0};
            return *this;
        }

        inline Material &MakeDiffuse(double diffusionFactor)
        {
            DiffusionFactor = diffusionFactor;
            return *this;
        }

        inline Material &MakeTotallyReflecting(double totalReflectionAngle)
        {
            CriticalAngle = totalReflectionAngle;
            return *this;
        }
    };
}