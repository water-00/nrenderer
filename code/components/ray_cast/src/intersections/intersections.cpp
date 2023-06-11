#include "intersections/intersections.hpp"

namespace RayCast::Intersection
{
    HitRecord xTriangle(const Ray& ray, const Triangle& t, float tMin, float tMax) {
        const auto& v1 = t.v1;
        const auto& v2 = t.v2;
        const auto& v3 = t.v3;
        const auto& normal = glm::normalize(t.normal);
        auto e1 = v2 - v1;
        auto e2 = v3 - v1;
        auto P = glm::cross(ray.direction, e2);
        float det = glm::dot(e1, P);
        Vec3 T;
        if (det > 0) T = ray.origin - v1;
        else { T = v1 - ray.origin; det = -det; }
        if (det < 0.000001f) return getMissRecord();
        float u, v, w;
        u = glm::dot(T, P);
        if (u > det || u < 0.f) return getMissRecord();
        Vec3 Q = glm::cross(T, e1);
        v = glm::dot(ray.direction, Q);
        if (v < 0.f || v + u > det) return getMissRecord();
        w = glm::dot(e2, Q);
        float invDet = 1.f / det;
        w *= invDet;
        if (w >= tMax || w <= tMin) return getMissRecord();
        return getHitRecord(w, ray.at(w), normal, t.material);

    }
    HitRecord xSphere(const Ray& ray, const Sphere& s, float tMin, float tMax) {
        const auto& position = s.position;
        const auto& r = s.radius;
        Vec3 oc = ray.origin - position;
        float a = glm::dot(ray.direction, ray.direction);
        float b = glm::dot(oc, ray.direction);
        float c = glm::dot(oc, oc) - r*r;
        float discriminant = b*b - a*c;
        float sqrtDiscriminant = sqrt(discriminant);
        if (discriminant > 0) {
            float temp = (-b - sqrtDiscriminant) / a;
            if (temp < tMax && temp > tMin) {
                auto hitPoint = ray.at(temp);
                auto normal = (hitPoint - position)/r;
                return getHitRecord(temp, hitPoint, normal, s.material);
            }
            temp = (-b + sqrtDiscriminant) / a;
            if (temp < tMax && temp > tMin) {
                auto hitPoint = ray.at(temp);
                auto normal = (hitPoint - position)/r;
                return getHitRecord(temp, hitPoint, normal, s.material);
            }
        }
        return getMissRecord();
    }
    HitRecord xPlane(const Ray& ray, const Plane& p, float tMin, float tMax) {
        Vec3 normal = glm::normalize(p.normal);
        auto Np_dot_d = glm::dot(ray.direction, normal);
        if (Np_dot_d < 0.0000001f && Np_dot_d > -0.00000001f) return getMissRecord();
        float dp = -glm::dot(p.position, normal);
        float t = (-dp - glm::dot(normal, ray.origin))/Np_dot_d;
        if (t >= tMax || t <= tMin) return getMissRecord();
        // cross test
        Vec3 hitPoint = ray.at(t);
        Mat3x3 d{p.u, p.v, glm::cross(p.u, p.v)};
        d = glm::inverse(d);
        auto res  = d * (hitPoint - p.position);
        auto u = res.x, v = res.y;
        if ((u<=1 && u>=0) && (v<=1 && v>=0)) {
            return getHitRecord(t, hitPoint, normal, p.material);
        }
        return getMissRecord();
    }
    HitRecord xAreaLight(const Ray& ray, const AreaLight& a, float tMin, float tMax) {
        Vec3 normal = glm::cross(a.u, a.v);
        Vec3 position = a.position;
        auto Np_dot_d = glm::dot(ray.direction, normal);
        if (Np_dot_d < 0.0000001f && Np_dot_d > -0.00000001f) return getMissRecord();
        float dp = -glm::dot(position, normal);
        float t = (-dp - glm::dot(normal, ray.origin))/Np_dot_d;
        if (t >= tMax || t <= tMin) return getMissRecord();
        // cross test
        Vec3 hitPoint = ray.at(t);
        Mat3x3 d{a.u, a.v, glm::cross(a.u, a.v)};
        d = glm::inverse(d);
        auto res  = d * (hitPoint - position);
        auto u = res.x, v = res.y;
        if ((u<=1 && u>=0) && (v<=1 && v>=0)) {
            return getHitRecord(t, hitPoint, normal, {});
        }
        return getMissRecord();
    }
}