#pragma once
#ifndef __SHADER_HPP__
#define __SHADER_HPP__

#include "geometry/vec.hpp"
#include "common/macros.hpp"
#include "scene/Scene.hpp"

namespace RayCast
{
    using namespace NRenderer;
    using namespace std;

    constexpr float PI = 3.1415926535898f;

    class Shader
    {
    protected:
        Material& material;
        vector<Texture>& textureBuffer;
    public:
        Shader(Material& material, vector<Texture>& textures)
            : material              (material)
            , textureBuffer         (textures)
        {}
        virtual RGB shade(const Vec3& in, const Vec3& out, const Vec3& normal) const = 0;
    };
    SHARE(Shader);
}

#endif