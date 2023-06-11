#pragma once
#ifndef __NR_CAMERA_HPP__
#define __NR_CAMERA_HPP__


#include <memory>

#include "geometry/vec.hpp"

namespace NRenderer
{
    using namespace std;
    struct Camera
    {
        Vec3 position; // 摄像机位置
        Vec3 up; // 摄像机的向上方向
        Vec3 lookAt; // 摄影机的目标方向
        float fov;
        float aperture; // 摄像机孔径
        float focusDistance; // 焦距
        float aspect; // 屏幕宽高比
        Camera() noexcept
            : position      (0.f, 0.f, 10.f)
            , up            (0.f, 1.f, 0.f)
            , lookAt        (0.f, 0.f, 1000.f)
            , fov           (40)
            , aperture      (0.f)
            , focusDistance (0.1f)
            , aspect        (1.f)
        {}
        Camera(
            const Vec3& position,
            const Vec3& up,
            const Vec3& lookAt,
            float fov,
            float aperture,
            float focusDistance,
            float aspect
        )
            : position      (position)
            , up            (up)
            , lookAt        (lookAt)
            , fov           (fov)
            , aperture      (aperture)
            , focusDistance (focusDistance)
            , aspect        (aspect)
        {}
    };
    using SharedCamera = shared_ptr<Camera>;
    
}

#endif
