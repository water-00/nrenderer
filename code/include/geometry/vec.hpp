#pragma once
#ifndef __NR_VEC_HPP__
#define __NR_VEC_HPP__

#include "glm/glm.hpp"
#include <iostream>
#include <iomanip>

namespace NRenderer
{
	using Index = unsigned int;

	class Handle
	{
	private:
		size_t value;
	public:
		Handle():value(0){};
		Handle(unsigned int i): value(i+1) {};
		size_t getValue() const { return value; }
		size_t index() const { return value - 1; }
		operator size_t() {	return value - 1; }
		bool valid() const { return value!=0; }
		void setIndex(unsigned int i) { value = i + 1; }
		void setValue(size_t v) { value = v; }
		void operator-- () { if (value != 0) value--; }
	};

	using Vec2 = glm::vec2;
	using Vec3 = glm::vec3;
	using Vec4 = glm::vec4;
	using Vec3d = glm::dvec3;

	using Mat4x4 = glm::mat4x4;
	using Mat3x3 = glm::mat3x3;

	using namespace std;

	// r,g,b's range: 0.0 - 1.0
	using RGB = Vec3;
	using RGBA = Vec4;

	// r,g,b's range: 0 - 255
	using RGBi = glm::u8vec3;
	using RGBAi = glm::u8vec4;
	
	static Vec3 Min(const Vec3& p1, const Vec3& p2) {
		return Vec3(std::min(p1.x, p2.x), std::min(p1.y, p2.y),
			std::min(p1.z, p2.z));
	}

	static Vec3 Max(const Vec3& p1, const Vec3& p2) {
		return Vec3(std::max(p1.x, p2.x), std::max(p1.y, p2.y),
			std::max(p1.z, p2.z));
	}
	
	static Vec3 Reflect(const Vec3& n1, const Vec3& n2) {
		return n1 - n2 * 2.0f * glm::dot(n1, n2); 
	}

	static float Max(const Vec3& c) {
		return std::max(c.x, (std::max(c.y, c.z)));
	}

	inline std::ostream& operator << (std::ostream &out, const Vec3& v) {
		return out<<"[ "<<v.x<<", "<<v.y<<", "<<v.z<<" ]";
	}
	inline std::ostream& operator << (std::ostream& out, const Vec3d& v) {
		return out << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";
	}
	inline std::ostream& operator << (std::ostream &out, const Vec4& v) {
		return out<<"[ "<<v.x<<", "<<v.y<<", "<<v.z<<", "<<v.w<<" ]";
	}
	inline void printMat4(const Mat4x4& mat) {
		int precision = 6;
		cout<<fixed<<setprecision(precision);
		cout<<" | "<<mat[0][0]<<", "<<mat[1][0]<<", "<<mat[2][0]<<", "<<mat[3][0]<<" | "<<endl;
		cout<<fixed<<setprecision(precision);
		cout<<" | "<<mat[0][1]<<", "<<mat[1][1]<<", "<<mat[2][1]<<", "<<mat[3][1]<<" | "<<endl;
		cout<<fixed<<setprecision(precision);
		cout<<" | "<<mat[0][2]<<", "<<mat[1][2]<<", "<<mat[2][2]<<", "<<mat[3][2]<<" | "<<endl;
		cout<<fixed<<setprecision(precision);
		cout<<" | "<<mat[0][3]<<", "<<mat[1][3]<<", "<<mat[2][3]<<", "<<mat[3][3]<<" | "<<endl;
	}

    inline
    float clamp(float target, float max = 1.f, float min = 0.f) {
        if (target > max) return max;
        if (target < min) return min;
        return target;
    }

	inline
	Vec3 clamp(const Vec3& v, float max = 1.f, float min = 0.f) {
		return {clamp(v.x, max, min), clamp(v.y, max, min), clamp(v.z, max, min)};
	}
	inline
	Vec4 clamp(const Vec4& v, float max = 1.f, float min = 0.f) {
		return {clamp(v.x, max, min), clamp(v.y, max, min), clamp(v.z, max, min), clamp(v.w, max, min)};
	}

    inline
    RGBi RGB2RGBi(const RGB& rgb) {
        return RGBi{ clamp(rgb.r)*float(255), clamp(rgb.g)*float(255), clamp(rgb.b)*float(255) };
    }

    inline
    RGBAi RGBA2RGBAi(const RGBA& rgba) {
        return RGBAi{ clamp(rgba.r)*float(255), clamp(rgba.g)*float(255), clamp(rgba.b)*float(255), clamp(rgba.a)*float(255) };
    }
}

#endif // !__NR_VEC_HPP__
