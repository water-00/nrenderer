#include "shaders/Microfacet.hpp"
#include "samplers/SamplerInstance.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/epsilon.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtx/compatibility.hpp"
#include "Onb.hpp"
#include "algorithm"
const float F0=0.04;
const float metalness = 0.2;
const float roughness = 0.2;
namespace AccPathTracer
{
    inline float SmithG1(const Vec3& v, const Vec3& h, const Vec3& n, float roughness) {
        double cos_v_n = glm::dot(v, n);
        if (cos_v_n * glm::dot(v, h) <= 0.0f)
        {
            return 0.f;
        }

        if (std::abs(cos_v_n - 1.0) < std::numeric_limits<double>::epsilon())
        {
            return 1.0f;
        }
        float cos_v_n_2 = pow(cos_v_n,2),
            tan_v_n_2 = (1.0f - cos_v_n_2) / cos_v_n_2,
            alpha_2 = roughness*roughness;
        return 2.0f / (1.0f + std::sqrt(1.0f + alpha_2 * tan_v_n_2));
    }
    inline Vec3 evalFresnel(Vec3 f0, float LdotH) {
        return f0 + (Vec3{1.0f,1.0f,1.0f} - f0) * (float)pow(1.0f - LdotH, 5);
    };
    inline Vec3 FresnelConductor(const Vec3& wi, const Vec3& normal, const Vec3& eta_r, const Vec3& eta_i) {
        float cos_theta_i = glm::dot(-wi, normal),
            cos_theta_i_2 = cos_theta_i * cos_theta_i,
            sin_theta_i_2 = 1.0f - cos_theta_i_2,
            sin_theta_i_4 = sin_theta_i_2 * sin_theta_i_2;

        Vec3 temp_1 = eta_r * eta_r - eta_i * eta_i - sin_theta_i_2,
            a_2_pb_2 = temp_1 * temp_1 + 4.0f * eta_i * eta_i * eta_r * eta_r;
        for (int i = 0; i < 3; i++)
        {
            a_2_pb_2[i] = std::sqrt(std::max(0.0f, a_2_pb_2[i]));
        }
        Vec3 a = 0.5f * (a_2_pb_2 + temp_1);
        for (int i = 0; i < 3; i++)
        {
            a[i] = std::sqrt(std::max(0.0f, a[i]));
        }
        Vec3 term_1 = a_2_pb_2 + cos_theta_i_2,
            term_2 = 2.0f * cos_theta_i * a,
            term_3 = a_2_pb_2 * cos_theta_i_2 + sin_theta_i_4,
            term_4 = term_2 * sin_theta_i_2,
            r_s = (term_1 - term_2) / (term_1 + term_2),
            r_p = r_s * (term_3 - term_4) / (term_3 + term_4);
        return 0.5f * (r_s + r_p);
    }

    /*inline Vec3 evalMicrofacet(brdfData data) {
        float D = GGX_D(data.alphaSquared, data.NdotH);
        float G2 = Smith_G2(data.alpha, data.NdotL, data.NdotV);

        return data.F * (G2 * D * data.NdotL);
    };*/
    inline Vec2 Sampler(unsigned int seed) {
        minstd_rand e_;
        e_.seed(seed);
        std::uniform_real_distribution<float> dist_ = std::uniform_real_distribution<float>(0.f, 1.f);
        return Vec2{ dist_(e_),dist_(e_) };
    }
    inline void CoordinateSystem(const Vec3& up, Vec3& B, Vec3& C)
    {
        if (std::fabs(up.x) > std::fabs(up.y))
        {
            double len_inv = 1.0f / std::sqrt(up.x * up.x + up.z * up.z);
            C = Vec3(up.z * len_inv, 0.f, -up.x * len_inv);
        }
        else
        {
            double len_inv = 1.0f / std::sqrt(up.y * up.y + up.z * up.z);
            C = Vec3(0.f, up.z * len_inv, -up.y * len_inv);
        }
        B = glm::cross(C, up);
    }
    //将单位向量从局部坐标系转换到世界坐标系
    inline Vec3 ToWorld(const Vec3& dir, const Vec3& up)
    {
        auto B = Vec3(0), C = Vec3(0);
        CoordinateSystem(up, B, C);
        return glm::normalize(dir.x * B + dir.y * C + dir.z * up);
    }
    inline void Sample(Vec3 n,float roughness, Vec3* H, float* pdf) {
        float sin_phi = 0.0, cos_phi = 0.0, alpha_2 = 0.0;
        Vec2 random_ = Sampler(6);
        float phi = 2.0f * PI * random_.y;
        cos_phi = std::cos(phi);
        sin_phi = std::sin(phi);
        alpha_2 = roughness*roughness;
        float tan_theta_2 = alpha_2 * random_.x / (1.0f - random_.x),
            cos_theta = 1.0 / std::sqrt(1.0f + tan_theta_2),
            sin_theta = std::sqrt(1.0f - cos_theta * cos_theta);
        *H = ToWorld({ sin_theta * cos_phi, sin_theta * sin_phi, cos_theta }, n);
        *pdf =1.0f / static_cast<float>(PI * alpha_2 * std::pow(cos_theta, 3) * pow(1.0f + tan_theta_2 / alpha_2,2));;
        return;
    }
    /*brdfData buildData(const Ray& ray, const Vec3& hitPoint, const Vec3& normal, RenderOption renderoption, Vec3 albedo, Vec3 direction) {
        brdfData data;
        //data.roughness = renderoption.roughness;
        //data.metalness = renderoption.metalness;
        data.roughness = roughness;
        data.metalness = metalness;
        //data.alpha = data.roughness * data.roughness;
        data.alpha = 0.5f;
        data.alphaSquared = data.alpha * data.alpha;
        //data.base_color = albedo;
        data.base_color= Vec3(0.9f, 0.4f, 0.4f);
        data.diffuseReflectance = data.base_color * (1.0f - data.metalness);
        //通过插值获取F0
        //glm::vec3 result = glm::lerp(glm::lowp_vec3(a), glm::lowp_vec3(b), glm::lowp_vec3(c));

        //data.specularF0 = (1 - data.metalness) * Vec3{ F0, F0, F0 } + data.metalness * data.base_color;
        data.specularF0 = Vec3(0.31f, 0.31f, 0.31f);

        //data.specularF0 = glm::lerp(Vec3{ F0, F0, F0 }, data.base_color, data.metalness);
        data.V = Vec3{ -(ray.direction.x),-(ray.direction.y),-(ray.direction.z) };
        data.L = direction;
        //查看随机生成的方向
        //cout << "L(" << L.x << "," << L.y << "," << L.z << ")" << endl;
        data.N = normal;
        data.H = glm::normalize(data.V+data.L);
        data.NdotL = glm::dot(data.N, data.L);
        data.NdotV = glm::dot(data.N, data.V);
        //cout << "NdotL:" << NdotL > 0 ? "true" : "false" << endl;
        //cout << "NdotV:" << NdotV > 0 ? "true" : "false" << endl;
        //Vbackfacing = (NdotV <= 0.0f);
        //Lbackfacing = (NdotL <= 0.0f);
        
        //需要确保点乘在0~1之间
        data.NdotL = glm::min(glm::max(0.0f, data.NdotL), 1.0f);
        data.NdotV = glm::min(glm::max(0.0f, data.NdotV), 1.0f);

        data.LdotH = saturate(glm::dot(data.L, data.H));
        data.NdotH = saturate(glm::dot(data.N, data.H));
        data.VdotH = saturate(glm::dot(data.V, data.H));

        //cout << "LdotH:" << glm::dot(data.V, data.H) << endl;
        data.F = evalFresnel(data.specularF0, data.VdotH);

        return data;
    }*/
    Microfacet::Microfacet(Material& material, vector<Texture>& textures)
        : Shader(material, textures)
    {
        auto albedo = material.getProperty<Property::Wrapper::RGBType>("albedo");
        if (albedo) this->albedo = (*albedo).value;
        else this->albedo = { 1, 1, 1 };

        auto roughness= material.getProperty<Property::Wrapper::FloatType>("roughness");
        if (roughness) this->roughness = (*roughness).value;
        else this->roughness =0.2f;

        auto F0 = material.getProperty<Property::Wrapper::FloatType>("F0");
        if (F0) this->F0 = (*F0).value;
        else this->F0 = 0.04;

    }
    inline Vec3 Reflect(const Vec3& wi, const Vec3& normal)
    {
        return glm::normalize(wi - 2.0f * glm::dot(wi, normal) * normal);
    }
    Scattered Microfacet::shade(const Ray& ray, const Vec3& hitPoint, const Vec3& normal)const {
        Vec3 H = Vec3(0.f);
        float D = 0.f;
        //采样入射光与出射光的中间法向量
        Vec3 N = glm::normalize(normal);
        Sample(N, roughness, &H, &D);
        H = glm::normalize(H);
        Vec3 V = -ray.direction;
        float pdf= D * std::abs(1.0f / (4.0f *glm::dot(ray.direction, H)));
        //cout << "pdf:" << pdf << endl;
        //令V H L同向
        //if (glm::dot(V, H) < 0.f) { H = -H; }
        Vec3 L = glm::normalize(Reflect(ray.direction, H));
        //Vec3 L = glm::normalize(-V + 2.f * glm::dot(V, H) * H);

        float cos_theta_i = glm::dot(L, N); //入射光线方向和宏观表面法线方向夹角的余弦
        //pdf=0或入射光从物体内部穿过或出射光从内部穿过
        if (pdf == 0.f || glm::dot(ray.direction, normal) >= 0.f|| cos_theta_i <= 0.f) {
            Scattered s;
            s.pdf = 0.f;
            s.attenuation = Vec3(0.f);
            //cout << "cos_theta_i:" << cos_theta_i << endl;
            //cout << "pdf:" << pdf << endl;
            return s;
        }
        Vec3 specularF0 = (1.f - metalness) * Vec3{ F0, F0, F0 } + metalness * albedo;
        //Vec3 F1=evalFresnel(specularF0, cos_theta_i);
        Vec3 F = evalFresnel(specularF0, abs(glm::dot(L, H)));
        //Vec3 eta_r = Vec3{ 0.15494, 0.11648, 0.13809 };
        //Vec3 eta_i = Vec3{ 4.81810, 3.11562, 2.14240 };
       // Vec3 F = FresnelConductor(-L, H, eta_r, eta_i);
        //cout << "F:" <<F<< endl;
        //cout << "F1:" << F1 << endl;
        //cout << "F2:" << F2 << endl;
        float cos_theta_o = abs(glm::dot(N, V));
        float G = SmithG1(L,H,N,roughness)* SmithG1(V, H, N, roughness);//阴影-遮蔽项
        //cout << "G:" << G << endl;
        //cout << "D:" << D << endl;
        
        Vec3 attenuation =( F * G * D) / std::abs(4.0f* cos_theta_o);
        attenuation = attenuation / pdf;

        attenuation *= albedo;
        
        //cout << "attenuation:" << attenuation << endl;
       
        return {
            Ray{hitPoint, L},
            attenuation,
            Vec3(0.f),
            pdf,
        };
    }

}