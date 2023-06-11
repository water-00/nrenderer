#pragma once
#ifndef __GLASS_HPP__
#define __GLASS_HPP__

#include "Shader.hpp"

namespace AccPathTracer
{
    class Glass : public Shader
    {
    private:
        Vec3 absorbed;
        float ior;
    public:
        Glass(Material& material, vector<Texture>& textures):Shader(material, textures) {
            auto ior = material.getProperty<Property::Wrapper::FloatType>("ior");
            if (ior) this->ior = (*ior).value;

            auto absorbed = material.getProperty<Property::Wrapper::RGBType>("absorbed");
            if (absorbed) this->absorbed = (*absorbed).value;
        }
        Scattered shade(const Ray& ray, const Vec3& hitPoint, const Vec3& normal) const;
    };
}

#endif