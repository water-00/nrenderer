#pragma once
#ifndef __NR_SCENE_VIEW_HPP__
#define __NR_SCENE_VIEW_HPP__

#include "View.hpp"

namespace NRenderer
{
    class SceneView: public View
    {
    private:
        virtual void drawBeginWindow() override;
        virtual void draw() override;
        void cameraSetting();
        void renderSetting();
        void ambientSetting();
        void componentSetting();

        int currComponentSelected;
    public:
        SceneView(const Vec2& position, const Vec2& size, UIContext& uiContext, Manager& manager);
    };
} // namespace NRenderer


#endif