#pragma once
#ifndef __NR_ASSET_HPP__
#define __NR_ASSET_HPP__

#include "common/macros.hpp"

#include "ModelItem.hpp"
#include "MaterialItem.hpp"
#include "TextureItem.hpp"
#include "LightItem.hpp"

namespace NRenderer
{
    struct Asset
    {
        vector<ModelItem> modelItems;
        vector<NodeItem> nodeItems;
        vector<MaterialItem> materialItems;
        vector<TextureItem> textureItems;
        vector<LightItem> lightItems;

        vector<SharedSphere> spheres;
        vector<SharedTriangle> triangles;
        vector<SharedPlane> planes;
        vector<SharedMesh> meshes;

        vector<SharedPointLight> pointLights;
        vector<SharedAreaLight> areaLights;
        vector<SharedDirectionalLight> directionalLights;
        vector<SharedSpotLight> spotLights;

        void clearModel() {
            for (auto& node : nodeItems) {
                if (node.glVAO != 0) {
                    glDeleteVertexArrays(1, &node.glVAO);
                }
                if (node.glVBO != 0) {
                    glDeleteBuffers(1, &node.glVBO);
                }
                if (node.glEBO != 0) {
                    glDeleteBuffers(1, &node.glEBO);
                }
            }
            for (auto& light : lightItems) {
                if (light.glVAO != 0) {
                    glDeleteVertexArrays(1, &light.glVAO);
                    light.glVAO = 0;
                }
                if (light.glVBO != 0) {
                    glDeleteBuffers(1, &light.glVBO);
                    light.glVBO = 0;
                }
                if (light.glEBO != 0) {
                    glDeleteBuffers(1, &light.glEBO);
                    light.glEBO = 0;
                }
            }
            modelItems.clear();
            nodeItems.clear();

            spheres.clear();
            triangles.clear();
            planes.clear();
            meshes.clear();

        }

        void clearLight() {
            lightItems.clear();

            pointLights.clear();
            areaLights.clear();
            directionalLights.clear();
            spotLights.clear();
        }

        void clearMaterial() {
            materialItems.clear();
        }

        void clearTexture() {
            textureItems.clear();
        }

        void genPreviewGlBuffersPerNode(NodeItem& node);
        void genPreviewGlBuffersPerLight(LightItem& light);

        void updateNodeGlDrawData(NodeItem& node);
        void updateLightGlDrawData(LightItem& light);
    };
    
} // namespace NRenderer


#endif