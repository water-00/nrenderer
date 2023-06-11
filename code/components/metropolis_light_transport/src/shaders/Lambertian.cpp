#include "shaders/Lambertian.hpp"
#include "samplers/SamplerInstance.hpp"

#include "Onb.hpp"

namespace Metropolis
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
        Vec3 direction = glm::normalize(onb.local(random));

        float pdf = 1/(2*PI);

        auto attenuation = albedo / PI;

        return {
            Ray{origin, direction},
            attenuation,
            Vec3{0},
            pdf
        };
    }
}