#pragma once
#ifndef __NR_SCENE_BUILDER_HPP__
#define __NR_SCENE_BUILDER_HPP__

#include "Asset.hpp"
#include "scene/Scene.hpp"
#include "manager/RenderSettingsManager.hpp"

namespace NRenderer
{
    class SceneBuilder
    {
    private:
    protected:
        const Asset& asset;
        const RenderSettings& renderSettings;
        const Camera& camera;
        const AmbientSettings& ambientSettings;
        SharedScene scene;
        void buildRenderOption();
        void buildBuffer();
        void buildCamera();
        void buildAmbient();
        bool success;
    public:
        SceneBuilder(const Asset& asset, const RenderSettings& renderSettings, const AmbientSettings& ambientSettings, const Camera& camera)
            : asset             (asset)
            , renderSettings    (renderSettings)
            , ambientSettings   (ambientSettings)
            , camera            (camera)
            , scene             (nullptr)
            , success           (true)
        {}
        SharedScene build();
    };
}

#endif