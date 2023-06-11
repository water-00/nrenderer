#pragma once
#ifndef __NR_MODEL_ITEM_HPP__
#define __NR_MODEL_ITEM_HPP__

#include "scene/Model.hpp"
#include "Item.hpp"

#include "glad/glad.h"

namespace NRenderer
{
    using namespace std;
    struct ModelItem : public Item
    {
        SharedModel model{nullptr};
    };

    struct NodeItem : public Item
    {
        GlId glVAO{0};
        GlId glVBO{0};
        GlId glEBO{0};
        SharedNode node{nullptr};
        SharedGlDrawData externalDrawData{nullptr};
    };
    
    
} // namespace NRenderer


#endif