#pragma once
#ifndef __SHADER_CREATOR_HPP__
#define __SHADER_CREATOR_HPP__

#include "Shader.hpp"
#include "Lambertian.hpp"
#include "Microfacet.hpp"
#include "Glass.hpp"
#include "Conductor.hpp"

namespace AccPathTracer
{
    class ShaderCreator
    {
    public:
        ShaderCreator() = default;
        SharedShader create(Material& material, vector<Texture>& t, RenderOption& renderoption) {
            SharedShader shader{ nullptr };

            switch (material.type)
            {
            case 0:
                shader = make_shared<Lambertian>(material, t);
                break;
            case 1:
                shader = make_shared<Conductor>(material, t);
                break;
            case 2:
                shader = make_shared<Glass>(material, t);
                break;
            case 3:
                shader = make_shared<Microfacet>(material, t);//微表面
                break;
            default:
                shader = make_shared<Lambertian>(material, t);
                break;
            }
        return shader;
    }
    };
}

#endif