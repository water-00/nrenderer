#include "shaders/Lambertian.hpp"
#include "samplers/SamplerInstance.hpp"

#include "Onb.hpp"

namespace SimplePathTracer
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
        // if (normal == Vec3{0, 0, 1}) {
        //     direction = random;
        // }
        // else if (normal == Vec3{0, 0, -1}) {
        //     direction = -random;
        // }
        // else {
        //     Vec3 z{0, 0, 1};
        //     float angle = -acos(glm::dot(z, normal));
        //     Vec3 axis =  glm::cross(normal, z);
        //     Mat4x4 rotate = glm::rotate(Mat4x4{1}, angle, axis);
        //     direction = rotate*Vec4{random, 1};
        // }
        // direction = glm::normalize(direction);

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