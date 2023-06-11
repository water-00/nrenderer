#pragma once
#ifndef __NR_MANAGER_HPP__
#define __NR_MANAGER_HPP__

#include "AssetManager.hpp"
#include "RenderSettingsManager.hpp"
#include "ComponentManager.hpp"

namespace NRenderer
{
    class Manager
    {
    public:
        AssetManager assetManager;
        RenderSettingsManager renderSettingsManager;
        ComponentManager componentManager;
        Manager();
        ~Manager() = default;
    };
} // namespace NRenderer


#endif