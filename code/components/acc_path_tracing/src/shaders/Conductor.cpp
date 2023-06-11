#include "shaders/Conductor.hpp"
#include "samplers/SamplerInstance.hpp"
#include "Onb.hpp"

namespace AccPathTracer {
	Scattered Conductor::shade(const Ray& ray, const Vec3& hitPoint, const Vec3& normal)const {
		//首先计算出射方向
		Vec3 V = -ray.direction;
		Vec3 N = glm::normalize(normal);
		Vec3 L = glm::normalize(-V + 2.f * glm::dot(V, N) * N);//L与N同向
		//计算导体的菲涅尔系数
		float cos_L = abs(glm::dot(L, N));
		float cos_L_2 = cos_L * cos_L;
		float sin_L_2 = 1 - cos_L_2;
		float sin_L_4 = sin_L_2 * sin_L_2;
		Vec3 temp_1 = eta_r * eta_r - eta_i * eta_i - sin_L_2,
			a_2_pb_2 = temp_1 * temp_1 + 4.0f * eta_i * eta_i * eta_r * eta_r;
		for (int i = 0; i < 3; i++)
		{
			a_2_pb_2[i] = sqrt(max(0.0f, a_2_pb_2[i]));
		}
		Vec3 a = 0.5f * (a_2_pb_2 + temp_1);
		for (int i = 0; i < 3; i++)
		{
			a[i] = sqrt(max(0.f, a[i]));
		}
		Vec3 term_1 = a_2_pb_2 + cos_L_2,
			term_2 = 2.f * cos_L * a,
			term_3 = a_2_pb_2 * cos_L_2 + sin_L_4,
			term_4 = term_2 * sin_L_2,
			r_s = (term_1 - term_2) / (term_1 + term_2),
			r_p = r_s * (term_3 - term_4) / (term_3 + term_4);
		Vec3 F=0.5f * (r_s + r_p);
		//
		//cout << "F:" << F << endl;
		Vec3 attenuation = F * abs(glm::dot(L, N)) * albedo;
		//cout << "attenuation "<<attenuation << endl;
		Scattered s;
		s.ray = Ray(hitPoint, L);
		s.attenuation = attenuation;
		return s;
	}
}