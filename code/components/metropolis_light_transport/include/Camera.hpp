#pragma once
#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include "scene/Camera.hpp"
#include "geometry/vec.hpp"

#include "samplers/SamplerInstance.hpp"

#include "Ray.hpp"

namespace Metropolis
{
    using namespace std;
    using namespace NRenderer;
    class Camera
    {
    private:


    public:
        const NRenderer::Camera& camera;
        double lenRadius; // 光圈半径
        double theta;
        Vec3d u, v, w; // w(0, 0, -1)向量表示相机的观察方向，u(-1, 0, 0)和v(0, 1, 0)向量与w垂直，用于表示相机的水平和垂直方向
        Vec3d vertical; // 相机的垂直方向
        Vec3d horizontal; // 相机的水平方向
        Vec3d lowerLeft; // 视平面左下角的位置
        Vec3d position; // 相机的位置
        double halfHeight;
        double focusDis;
        // double dist = 0.0f; // 相机到成像平面的距离

        Camera(const NRenderer::Camera& camera)
            : camera                (camera)
        {
            position = camera.position;
            lenRadius = camera.aperture / 2.f;
            auto vfov = camera.fov;
            vfov = clamp(vfov, 160.f, 20.f);
            theta = glm::radians(vfov);
            halfHeight = tan(theta/2.f);
            auto halfWidth = camera.aspect*halfHeight;
            Vec3d up = camera.up;
            w = glm::normalize(camera.position - camera.lookAt); // 这个定义得到(0, 0, -1)...和相机发射光线的方向是反的
            u = glm::normalize(glm::cross(up, w));
            v = glm::cross(w, u);

            focusDis = camera.focusDistance; // 焦距

            lowerLeft = position - halfWidth*focusDis*u - halfHeight*focusDis*v - focusDis*w; // 相机位置减去成像平面左下角的位置
            horizontal = 2*halfWidth*focusDis*u;
            vertical = 2*halfHeight*focusDis*v;
            // 在focusDistance = 0.1的情况下
            // cout << "lowerLeft = " << lowerLeft << ", horizontal = " << horizontal << ", vertical = " << vertical;
            // lowerLeft = [0.036397, -0.036397, 10.1], horizontal = [-0.072794, 0, 0], vertical = [0, 0.072794, 0]
            
        }

        // 从摄像机中发射光线，s t是单位正方形内采样得到的
        Ray shoot(double s, double t) const {
            auto r = defaultSamplerInstance<UniformInCircle>().sample2d();
            double rx = r.x * lenRadius;
            double ry = r.y * lenRadius;
            Vec3d offset = u*rx + v*ry;
            return Ray{
                position + offset,
                // 事实上focusDistance不会影响方向，因为它们三项都有因子focusDistance，normalize后是一样的
                glm::normalize(
                    lowerLeft + s*horizontal + t*vertical - (position + offset)
                )
            };
        }
    };
}

#endif