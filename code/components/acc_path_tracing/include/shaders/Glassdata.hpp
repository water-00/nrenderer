#pragma once
#ifndef __GLASSDATA_HPP__
#define __GLASSDATA_HPP__

#include "Ray.hpp"

namespace AccPathTracer
{
    struct GlassData
    {
        Ray reflex = {};
        Vec3 reflex_rate = {};
        Ray refraction = {};
        Vec3 refraction_rate = {};
    };

}

#endif