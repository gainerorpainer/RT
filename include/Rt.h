#pragma once

#include "Bitmap.h"
#include "Transformation.h"
#include "Camera.h"
#include "Scene.h"

namespace Rt
{
    constexpr unsigned int BOUNCE_ITERATIONS = 5;
    constexpr unsigned int DIFFUSE_RAYS = 5;

    Bitmap::Bitmap RT();
}