#pragma once
#ifndef __SAMPLER_INSTANCE_HPP__
#define __SAMPLER_INSTANCE_HPP__

#include "HemiSphere.hpp"
#include "Marsaglia.hpp"
#include "UniformSampler.hpp"
#include "UniformInCircle.hpp"
#include "UniformInSquare.hpp"

namespace SimplePathTracer
{
    template<typename T>
    T& defaultSamplerInstance() {
        static_assert(
            is_base_of<Sampler1d, T>::value ||
            is_base_of<Sampler2d, T>::value ||
            is_base_of<Sampler3d, T>::value, "Not a sampler type.");
        thread_local static T t{};
        return t;
    }
}

#endif