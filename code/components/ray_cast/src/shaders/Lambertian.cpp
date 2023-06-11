#include "shaders/Lambertian.hpp"

namespace RayCast
{
    Lambertian::Lambertian(Material& material, vector<Texture>& textures)
        : Shader                (material, textures)
    {
        auto optDiffuseColor = material.getProperty<Property::Wrapper::RGBType>("diffuseColor");
        if (optDiffuseColor) diffuseColor = (*optDiffuseColor).value;
        else diffuseColor = {1, 1, 1};
    }
    RGB Lambertian::shade(const Vec3& in, const Vec3& out, const Vec3& normal) const {
        return diffuseColor * glm::dot(out, normal);
    }
}