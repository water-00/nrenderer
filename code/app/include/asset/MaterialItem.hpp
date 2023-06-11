#pragma once
#ifndef __NR_MATERIAL_ITEM_HPP__
#define __NR_MATERIAL_ITEM_HPP__

#include "scene/Material.hpp"
#include "Item.hpp"

namespace NRenderer
{
    struct MaterialItem : public Item
    {
        SharedMaterial material;
    };
} // namespace NRenderer


#endif