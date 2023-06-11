#ifndef __BOUNDS3_HPP__
#define __BOUNDS3_HPP__

#include <vector>
#include <list>
#include <algorithm>
#include <array>
#include "scene/Scene.hpp"
#include "Ray.hpp"

using namespace NRenderer;
namespace Metropolis {
    class Bounds3 {
    public:
        // two points to specify the bounding box
        Vec3 min, max;
        int node_id;
        enum class Type
        {
            SPHERE = 0x1,
            TRIANGLE = 0X2,
            PLANE = 0X3,
            MESH = 0X4
        };
        Bounds3() {
            double minNum = std::numeric_limits<double>::lowest();
            double maxNum = std::numeric_limits<double>::max();
            max = Vec3(minNum, minNum, minNum);
            min = Vec3(maxNum, maxNum, maxNum);
        }
        Bounds3(Sphere* sp, Mat4x4 t, int id) {
            node_id = id;
            type = Type::SPHERE;
            this->sp = sp;
            float r = sp->radius;
            Vec3 pos = t * Vec4{ sp->position, 1 };
            min = Vec3{ pos.x - r, pos.y - r, pos.z - r };
            max = Vec3{ pos.x + r, pos.y + r, pos.z + r };
        }
        Bounds3(Triangle* tr, Mat4x4 t, int id) {
            node_id = id;
            type = Type::TRIANGLE;
            this->tr = tr;
            Vec3 v1 = t * Vec4{ tr->v1 , 1 }, v2 = t * Vec4{ tr->v2 , 1 }, v3 = t * Vec4{ tr->v3 , 1 };
            min = Vec3{ std::min(v1.x, std::min(v2.x, v3.x)),
                std::min(v1.y, std::min(v2.y, v3.y)),
                std::min(v1.z, std::min(v2.z, v3.z)) };

            max = Vec3{ std::max(v1.x, std::max(v2.x, v3.x)),
                std::max(v1.y, std::max(v2.y, v3.y)),
                std::max(v1.z, std::max(v2.z, v3.z)) };
        }
        Bounds3(Plane* pl, Mat4x4 t, int id) {
            node_id = id;
            // 给平面建BoundingBox
            type = Type::PLANE;
            this->pl = pl;
            float epsilon = 0.01f;

            // 求出平面的四个点
            Vec3 p1 = pl->position;
            Vec3 p2 = p1 + pl->u;
            Vec3 p3 = p1 + pl->v;
            Vec3 p4 = p1 + pl->u + pl->v;
            p1 = t * Vec4{ p1, 1 };
            p2 = t * Vec4{ p2, 1 };
            p3 = t * Vec4{ p3, 1 };
            p4 = t * Vec4{ p4, 1 };

            Vec3 n = pl->normal;

            // 思路：将四个点分别沿相反的法线方向移动一点距离，构造BoundingBox
            p1 -= epsilon * n;
            p2 -= epsilon * n;
            p3 += epsilon * n;
            p4 += epsilon * n;
            float minX = std::min(p1.x, std::min(p2.x, std::min(p3.x, p4.x)));
            float minY = std::min(p1.y, std::min(p2.y, std::min(p3.y, p4.y)));
            float minZ = std::min(p1.z, std::min(p2.z, std::min(p3.z, p4.z)));
            float maxX = std::max(p1.x, std::max(p2.x, std::max(p3.x, p4.x)));
            float maxY = std::max(p1.y, std::max(p2.y, std::max(p3.y, p4.y)));
            float maxZ = std::max(p1.z, std::max(p2.z, std::max(p3.z, p4.z)));
            min = Vec3{ minX, minY, minZ };
            max = Vec3{ maxX, maxY, maxZ };
            // cout << "min: " << min << ", max: " << max << endl;
        }
        Bounds3(Mesh* ms) {
            type = Type::MESH;
            this->ms = ms;
            // Mesh类型不用建BoundingBox
        }

        Vec3 Centroid() {
            return 0.5f * min + 0.5f * max;
        }

        // BoundingBox对角线向量
        Vec3 Diagonal() const { return max - min; }
        inline bool IntersectP(const Ray& ray, const Vec3& invDir,
            const std::array<int, 3>& dirisNeg);
        int maxExtent() const
        {
            Vec3 d = Diagonal();
            if (d.x > d.y && d.x > d.z)
                return 0;
            else if (d.y > d.z)
                return 1;
            else
                return 2;
        }

        // 判断点p是否在包围盒b内
        bool Inside(const Vec3& p, const Bounds3& b) const
        {
            return (p.x >= b.min.x && p.x <= b.max.x && p.y >= b.min.y &&
                p.y <= b.max.y && p.z >= b.min.z && p.z <= b.max.z);
        }

        union
        {
            Sphere* sp;
            Triangle* tr;
            Plane* pl;
            Mesh* ms;
        };
        Type type;

    };
    inline bool Bounds3::IntersectP(const Ray& ray, const Vec3& invDir, const std::array<int, 3>& dirIsNeg = { 0, 0, 0 }) {
        if (Inside(ray.origin, *this)) {
            return true;
        }

        float t1_min, t1_max, t2_min, t2_max, t3_min, t3_max;
        t1_min = (min[0] - ray.origin[0]) * invDir[0];
        t2_min = (min[1] - ray.origin[1]) * invDir[1];
        t3_min = (min[2] - ray.origin[2]) * invDir[2];
        t1_max = (max[0] - ray.origin[0]) * invDir[0];
        t2_max = (max[1] - ray.origin[1]) * invDir[1];
        t3_max = (max[2] - ray.origin[2]) * invDir[2];

        // 如果direction某一维度为负，那么当这个维度上的O->pMax长度大于O->pMin（假设两个值都为正）时，会得到 tmax < tmin < 0
        if (ray.direction[0] < 0) std::swap(t1_min, t1_max);
        if (ray.direction[1] < 0) std::swap(t2_min, t2_max);
        if (ray.direction[2] < 0) std::swap(t3_min, t3_max);


        float t_near = std::max(t1_min, std::max(t2_min, t3_min));
        float t_far = std::min(t1_max, std::min(t2_max, t3_max));


        if (t_far >= 0 && t_near < t_far) {
          return true;
        }
        return false;
    }
}

#endif