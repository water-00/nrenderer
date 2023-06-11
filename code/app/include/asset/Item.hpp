#pragma once
#ifndef __NR_ITEM_HPP__
#define __NR_ITEM_HPP__

#include <string>

namespace NRenderer
{
    using namespace std;
    using GlId = unsigned int;
    struct Item
    {
        string name;
        Item()
            : name      ("undefined")
        {}
    };
    struct GlDrawData
    {
        vector<Vec3> positions;
    };
    SHARE(GlDrawData);
}

#endif