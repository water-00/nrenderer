#pragma once
#ifndef __PHONG_HPP__
#define __PHONG_HPP__

#include "Shader.hpp"

namespace RayCast
{
    class Phong : public Shader
    {
    private:
        Vec3 diffuseColor;
        Vec3 specularColor;
        float specularEx;
    public:
        Phong(Material& material, vector<Texture>& textures);
        virtual RGB shade(const Vec3& in, const Vec3& out, const Vec3& normal) const;
    };
}

#endif