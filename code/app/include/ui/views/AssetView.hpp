#pragma once
#ifndef __NR_ASSET_VIEW_HPP__
#define __NR_ASSET_VIEW_HPP__

#include "View.hpp"

namespace NRenderer
{
    class AssetView: public View
    {
    private:
        virtual void drawBeginWindow() override;
        virtual void draw() override;
        void menu();
        void modelTab();
        void materialTab();
        void textureTab();
        void lightTab();

        int currMtlIndex;
        int currLightIndex;

        Property tempProp;
        int tempPropTypeIndex;
        bool isPropKeyValid;
        void tempPropEditor();

        void resetTempProp();

        MaterialItem tempMaterialItem;

        void materialPropEditor(SharedMaterial spMaterial);

    public:
        AssetView(const Vec2& position, const Vec2& size, UIContext& uiContext, Manager& manager);
    };
} // namespace NRenderer


#endif