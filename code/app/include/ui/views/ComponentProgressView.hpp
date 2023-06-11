#pragma once
#ifndef __NR_COMPONENT_PROGRESS_VIEW_HPP__
#define __NR_COMPONENT_PROGRESS_VIEW_HPP__

#include "View.hpp"

namespace NRenderer
{
   class ComponentProgressView: public View
   {
    private:
        virtual void drawSetup();
        virtual void drawFinish();
        virtual void drawPosAndSize();
        virtual void drawBeginWindow();
        virtual void drawEndWindow();
        virtual void draw();
    public:
        using View::View;
   };
}

#endif