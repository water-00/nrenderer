#pragma once
#ifndef __NR_LOG_VIEW_HPP__
#define __NR_LOG_VIEW_HPP__

#include "View.hpp"

namespace NRenderer
{
    class LogView : public View
    {
    protected:
        virtual void draw() override;
        virtual void drawBeginWindow() override;
    public:
        using View::View;
    };
} // namespace NRenderer


#endif