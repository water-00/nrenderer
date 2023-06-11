#pragma once
#ifndef __CONDUCTOR_HPP__
#define __CONDUCTOR_HPP__

#include "Shader.hpp"

namespace AccPathTracer
{
    class Conductor : public Shader
    {
    //假设该导体是理想光滑的情况，所以是全反射
    private:
        Vec3 albedo; //金属原色
        Vec3 eta_r;                     //材质相对折射率的实部
        Vec3 eta_i;                       //材质相对折射率的虚部（消光系数）
    public:
        Conductor(Material& material, vector<Texture>& textures) :Shader(material, textures) {
            auto albedo = material.getProperty<Property::Wrapper::RGBType>("albedo");
            if (albedo) this->albedo = (*albedo).value;

            auto eta_ = material.getProperty<Property::Wrapper::Vec3Type>("eta_r");
            if (eta_) this->eta_r = (*eta_).value;

            auto eta_i = material.getProperty<Property::Wrapper::Vec3Type>("eta_i");
            if (eta_i) this->eta_i = (*eta_i).value;
        }
        Scattered shade(const Ray& ray, const Vec3& hitPoint, const Vec3& normal) const;
    };
}

#endif