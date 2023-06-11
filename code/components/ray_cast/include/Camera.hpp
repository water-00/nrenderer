#pragma once
#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include "scene/Camera.hpp"
#include "geometry/vec.hpp"

#include "Ray.hpp"

namespace RayCast
{
    using namespace std;
    using namespace NRenderer;
    class Camera
    {
    private:
        const NRenderer::Camera& camera;
        float lenRadius;
        Vec3 u, v, w;
        Vec3 vertical;
        Vec3 horizontal;
        Vec3 lowerLeft;
        Vec3 position;
    public:
        Camera(const NRenderer::Camera& camera)
            : camera                (camera)
        {
            position = camera.position;
            lenRadius = camera.aperture / 2.f;
            auto vfov = camera.fov;
            vfov = clamp(vfov, 160.f, 20.f);
            auto theta = glm::radians(vfov);
            auto halfHeight = tan(theta/2.f);
            auto halfWidth = camera.aspect*halfHeight;
            Vec3 up = camera.up;
            w = glm::normalize(camera.position - camera.lookAt);
            u = glm::normalize(glm::cross(up, w));
            v = glm::cross(w, u);

            auto focusDis = camera.focusDistance;

            lowerLeft = position - halfWidth*focusDis*u
                - halfHeight*focusDis*v
                - focusDis*w;
            horizontal = 2*halfWidth*focusDis*u;
            vertical = 2*halfHeight*focusDis*v;
        }

        // 从摄像机中发射光线
        Ray shoot(float s, float t) const {
            return Ray{
                position,
                glm::normalize(
                    lowerLeft + s*horizontal + t*vertical - position
                )
            };
        }
    };
}

#endif