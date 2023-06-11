#pragma once
#ifndef __UNIFORM_IN_CIRCLE_HPP__
#define __UNIFORM_IN_CIRCLE_HPP__

#include "Sampler2d.hpp"

namespace SimplePathTracer
{
    using namespace std;
    class UniformInCircle : public Sampler2d
    {
    private:
        default_random_engine e;
        uniform_real_distribution<float> u;
    public:
        UniformInCircle()
            : e               ((unsigned int)time(0) + insideSeed())
            , u               (-1, 1)
        {}
        Vec2 sample2d() override {
            float x{0}, y{0};
            do {
                x = u(e);
                y = u(e);
            } while((x*2 + y*2) > 1);
            return { x, y };
        }
    
    };
}

#endif