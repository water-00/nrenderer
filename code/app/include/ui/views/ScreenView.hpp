#pragma once
#ifndef __NR_SCREEN_VIEW_HPP__
#define __NR_SCREEN_VIEW_HPP__

#include "View.hpp"
#include "utilities/GlImage.hpp"

#include "utilities/GlShader.hpp"

namespace NRenderer
{
    class ScreenView: public View
    {
    private:
        virtual void drawBeginWindow() override;
        virtual void draw() override;
        void drawGlPreview();
        void preview();
        void result();
        float getShrinkNum();

        GlShader nodeShader;
        GlShader lightShader;

        GlId glFBO;
        GlId glRBO;

        unsigned int prePreviewHeight;
        unsigned int prePreviewWidth;

        unsigned int previewHeight;
        unsigned int previewWidth;

        enum class CoordinateType {
            LEFT_HANDED,
            RIGHT_HANDED
        };
        CoordinateType previewCoordinateType;

        bool isPreviewSizeChange();
        void updatePreviewSize(unsigned int x, unsigned int y);
        void updatePrePreviewSize();

        void genFB();
        void previewNode(const NodeItem& n);
        void previewLight(const LightItem& l);

        void align(const Vec2& size);

        enum class ViewType
        {
            PREVIEW,
            RESULT
        };

        ViewType viewType;

        int shrinkLevel;

        GlImageId renderResult;
        GlImageId previewResult;
    public:
        ScreenView(const Vec2& position, const Vec2& size, UIContext& uiContext, Manager& manager);
    };
} // namespace NRenderer


#endif