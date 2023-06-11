#pragma once
#ifndef __NR_RENDER_COMPONENT_HPP__
#define __NR_RENDER_COMPONENT_HPP__

#include "Component.hpp"
#include "scene/Scene.hpp"

#include <functional>

namespace NRenderer
{
    class DLL_EXPORT RenderComponent: public Component
    {
    private:
        virtual void render(SharedScene spScene) = 0;
    public:
        void exec(function<void()> onStart, function<void()> onFinish, SharedScene spScene);
    };
}

#define REGISTER_RENDERER(__NAME__, __DESCRIPTION__, __CLASS__)          REGISTER_COMPONENT(Render, __NAME__, __DESCRIPTION__, __CLASS__)
#define REGISTER_RENDERER_NO_DESCRIPTION(__NAME__, __CLASS__)            REGISTER_RENDERER(__NAME__, "" ,__CLASS__)
#endif