#pragma once
#ifndef __RAY_HPP__
#define __RAY_HPP__

#include "geometry/vec.hpp"

#include <limits>

#define FLOAT_INF numeric_limits<float>::infinity()
namespace SimplePathTracer
{
    using namespace NRenderer;
    using namespace std;


    struct Ray
    {
        Vec3 origin;
        // keep it as a unit vector
        Vec3 direction;

        void setOrigin(const Vec3& v) {
            origin = v;
        }

        void setDirection(const Vec3& v) {
            direction = glm::normalize(v);
        }

        inline
        Vec3 at(float t) const {
            return origin + t*direction;
        }

        Ray(const Vec3& origin, const Vec3& direction)
            : origin                (origin)
            , direction             (direction)
        {}
    
        Ray()
            : origin        {}
            , direction     {}
        {}
    };
}

#endif