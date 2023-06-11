#pragma once
#ifndef __SAMPLER_1D_HPP__
#define __SAMPLER_1D_HPP__

#include "Sampler.hpp"

#include <random>

namespace SimplePathTracer
{
    class Sampler1d : protected Sampler
    {
    public:
        Sampler1d() = default;
        virtual float sample1d() = 0;
    };
}

#endif