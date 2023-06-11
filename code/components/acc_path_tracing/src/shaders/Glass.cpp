#include "shaders/Glass.hpp"
#include "samplers/SamplerInstance.hpp"
#include "Onb.hpp"

namespace AccPathTracer 
{
    /*Glass::Glass(Material& material, vector<Texture>& textures) :Shader(material, textures) {
        auto ior = material.getProperty<Property::Wrapper::FloatType>("ior");
        if (ior) this->ior = (*ior).value;

        auto absorbed = material.getProperty<Property::Wrapper::RGBType>("absorbed");
        if (absorbed) this->absorbed = (*absorbed).value;
    }*/
    
    Scattered Glass::shade(const Ray& ray, const Vec3& hitPoint, const Vec3& normal) const {
        Vec3 origin = hitPoint;
        Vec3 N = glm::normalize(normal);
        Vec3 V = glm::normalize(ray.direction);

        float ior_inverse;
        ior_inverse = ior;
        //如果入射光与法线方向相同，则令法线取反
        if (glm::dot(V, N) > 0.f) {
            N = -N;
            ior_inverse = 1/ior;
        }

        Vec3 reflex = glm::normalize(V + N*2.f*glm::dot(-V,N));//反射光
        //计算基础反射率F0
        float n12 = (ior_inverse - 1.f) / (ior_inverse + 1.f);
        n12 = n12 * n12;
        Vec3 F0 = Vec3(n12);
        //菲涅尔Schlick计算反射率
        float VdotN = abs(glm::dot(V, N));
        Vec3 F = (F0 + (Vec3(1.f) - F0) * (float)pow(1 - VdotN, 5));
        Vec3 reflex_rate = F * absorbed;
        Vec3 refraction_rate = (1.f - F) * absorbed;
        //建立局部坐标系
        Vec3 x = glm::normalize(reflex + V);
        Vec3 y = glm::normalize(-N);
        //计算折射相对入射角在x,y轴上的分量
        float x_ = sqrt(pow(1 - abs(glm::dot(V, N)), 2))/ior_inverse;
        float y_ = sqrt(1 - pow(x_, 2));
        //计算折射角方向
        Vec3 refraction = glm::normalize(x * x_ + y * y_);
        
        if (x_ > 1.f) {
            reflex = absorbed;
            refraction_rate = Vec3(0.f);
            refraction = Vec3(0.f);
        }
        Scattered ret;
        ret.data = { Ray{origin,reflex},reflex_rate,Ray{origin,refraction},refraction_rate };

        return ret;

    }
}