#pragma once
#ifndef __NR_LIGHT_ITEM_HPP__
#define __NR_LIGHT_ITEM_HPP__

#include "scene/Light.hpp"
#include "Item.hpp"

namespace NRenderer
{
    using namespace std;
    struct LightItem : public Item
    {
        GlId glVAO{0};
        GlId glVBO{0};
        GlId glEBO{0};
        SharedLight light;
        SharedGlDrawData externalDrawData;
    };
    
    
} // namespace NRenderer


#endif