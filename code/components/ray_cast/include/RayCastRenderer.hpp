#pragma once
#ifndef __RAY_CAST_HPP__
#define __RAY_CAST_HPP__

#include "scene/Scene.hpp"

#include "Camera.hpp"
#include "intersections/intersections.hpp"

#include "shaders/ShaderCreator.hpp"

namespace RayCast
{
    using namespace NRenderer;
    class RayCastRenderer
    {
    private:
        SharedScene spScene;
        Scene& scene;
        RayCast::Camera camera;

        vector<SharedShader> shaderPrograms;
    public:
        RayCastRenderer(SharedScene spScene)
            : spScene               (spScene)
            , scene                 (*spScene)
            , camera                (spScene->camera)
        {}
        ~RayCastRenderer() = default;

        using RenderResult = tuple<RGBA*, unsigned int, unsigned int>;
        RenderResult render();
        void release(const RenderResult& r);

    private:
        RGB gamma(const RGB& rgb);
        RGB trace(const Ray& r);
        HitRecord closestHit(const Ray& r);
    };
}

#endif