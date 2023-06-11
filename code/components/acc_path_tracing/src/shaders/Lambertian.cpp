#include "shaders/Lambertian.hpp"
#include "samplers/SamplerInstance.hpp"

#include "Onb.hpp"

namespace AccPathTracer
{
    Lambertian::Lambertian(Material& material, vector<Texture>& textures)
        : Shader                (material, textures)
    {
        auto diffuseColor = material.getProperty<Property::Wrapper::RGBType>("diffuseColor");
        if (diffuseColor) albedo = (*diffuseColor).value;
        else albedo = {1, 1, 1};
    }

    Scattered Lambertian::shade(const Ray& ray, const Vec3& hitPoint, const Vec3& normal) const {
        Vec3 origin = hitPoint;
        Vec3 random = defaultSamplerInstance<HemiSphere>().sample3d();

        Onb onb{normal};
        Vec3 direction = glm::normalize(onb.local(random)); // direction沿以normal方向为中心的半球均匀分布

        float pdf = 1/(2*PI);

        //albedo: 表面对不同颜色的反射率
        //attenuation：光线在经过特定albedo后的衰减比例
        auto attenuation = albedo / PI;
        return {
            Ray{origin, direction},//发出光线
            attenuation,
            Vec3{0},
            pdf,
        };
    }
}