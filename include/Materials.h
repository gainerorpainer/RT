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
        FloatingType_t DiffusionFactor = 0;

        /// @brief Indicent angle towards surface normal at which (higher than this value) total reflection occurs
        FloatingType_t CriticalAngle = Deg2Rad(90);

        inline Material &MakeEmissive(Color_t emissionSpectrum)
        {
            Emission = emissionSpectrum;
            IsLightsource = true;
            return *this;
        }

        inline Material &MakeAbsorbing(Color_t visibleColor)
        {
            ColorFilter = ColorD_t{(FloatingType_t)visibleColor[0] / FloatingType_t{255}, (FloatingType_t)visibleColor[1] / FloatingType_t{255}, (FloatingType_t)visibleColor[2] / FloatingType_t{255}};
            return *this;
        }

        inline Material &MakeDiffuse(FloatingType_t diffusionFactor)
        {
            DiffusionFactor = diffusionFactor;
            return *this;
        }

        inline Material &MakeTotallyReflecting(FloatingType_t totalReflectionAngle)
        {
            CriticalAngle = totalReflectionAngle;
            return *this;
        }
    };
}