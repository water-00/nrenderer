#pragma once
#ifndef __NR_TEXTURE_ITEM_HPP__
#define __NR_TEXTURE_ITEM_HPP__

#include "scene/Texture.hpp"
#include "Item.hpp"
#include "utilities/GlImage.hpp"
namespace NRenderer
{
    struct TextureItem : public Item
    {
        SharedTexture texture;
        GlImageId glId;
    };
} // namespace NRenderer


#endif