#include "ui/views/SceneView.hpp"
#include "asset/SceneBuilder.hpp"

namespace NRenderer
{
    SceneView::SceneView(const Vec2& position, const Vec2& size, UIContext& uiContext, Manager& manager)
        : View                  (position, size, uiContext, manager)
        , currComponentSelected (-1)
    {
    }

    void SceneView::drawBeginWindow() {
        ImGui::Begin("Render Settings", nullptr, windowFlag);
    }
    void SceneView::draw() {
        cameraSetting();
        ImGui::Separator();
        renderSetting();
        ImGui::Separator();
        ambientSetting();
        ImGui::Separator();
        componentSetting();
    }
    void SceneView::cameraSetting() {
        auto& camera = manager.renderSettingsManager.camera;
        ImGui::TextUnformatted("Camera:");
        float floatStep = 0.1;
        ImGui::InputFloat3("Position", &camera.position.x);
        ImGui::InputFloat3("Up", &camera.up.x);
        ImGui::InputFloat3("LookAt", &camera.lookAt.x);
        ImGui::InputScalar("Fov", ImGuiDataType_Float, &camera.fov, &floatStep, NULL);
        ImGui::InputScalar("Aspect", ImGuiDataType_Float, &camera.aspect, &floatStep, NULL);
        ImGui::InputScalar("Aperture", ImGuiDataType_Float, &camera.aperture, &floatStep, NULL);
        ImGui::InputScalar("FocusDistance", ImGuiDataType_Float, &camera.focusDistance, &floatStep, NULL);
    }
    void SceneView::renderSetting() {
        int intStep = 1;
        double doubleStep = 0.01;
        auto& rs = manager.renderSettingsManager.renderSettings;
        ImGui::TextUnformatted("Render Settings:");
        ImGui::InputScalar("Width", ImGuiDataType_U32, &rs.width, &intStep, NULL, "%u");
        ImGui::InputScalar("Height", ImGuiDataType_U32, &rs.height, &intStep, NULL, "%u");
        ImGui::InputScalar("Depth", ImGuiDataType_U32, &rs.depth, &intStep, NULL, "%u");
        ImGui::InputScalar("Sample Nums", ImGuiDataType_U32, &rs.samplesPerPixel, &intStep, NULL, "%u");
    }
    void SceneView::ambientSetting() {
        auto& as = manager.renderSettingsManager.ambientSettings;
        ImGui::TextUnformatted("Ambient:");
        const string typeStr[2] = {"Constant", "Environment Map"};
        int curr = as.type == AmbientSettings::Type::CONSTANT ? 0 : 1;
        if(ImGui::BeginCombo("Type##AmbientSettings", typeStr[curr].c_str())) {
            for (int i=0; i<2; i++) {
                bool selected = curr == i;
                if (ImGui::Selectable((typeStr[i]+"##AmbientTypeItem").c_str(), &selected)) {
                    as.type = i == 0 ? AmbientSettings::Type::CONSTANT : AmbientSettings::Type::ENVIROMENT_MAP;
                    curr = i;
                }    
            }
            ImGui::EndCombo();
        }
        if (curr == 0) {
            ImGui::ColorEdit3("Ambient", &as.ambient.r, ImGuiColorEditFlags_Float);
        }
        else if (curr == 1) {
            auto& v = as.mapTexture;
            if (ImGui::BeginCombo("Map Texture##AmbientEMAPTex", v.valid()? manager.assetManager.asset.textureItems[v.index()].name.c_str() : "")) {
                for (int i=0; i < manager.assetManager.asset.textureItems.size(); i++) {
                    bool selected = v.valid() ? (i == v.index()) : false;
                    if (ImGui::Selectable(manager.assetManager.asset.textureItems[i].name.c_str(), &selected)) {
                        v.setIndex(i);
                    }
                }
                ImGui::EndCombo();
            }
        }
    }
    void SceneView::componentSetting() {
        ImGui::TextUnformatted("Render Component:");
        auto&& components = getServer().componentFactory.getComponentsInfo("Render");
        auto& componentManager = manager.componentManager;
        auto& state = uiContext.state;
        string comboPre = currComponentSelected != -1 && currComponentSelected < components.size() ?
            components[currComponentSelected].name : "";
        if (ImGui::BeginCombo("Render Component##SceneView", comboPre.c_str())) {
            for(int i=0; i<components.size(); i++) {
                bool selected = i == currComponentSelected;
                if (ImGui::Selectable((to_string(i+1) + ". " + components[i].name + "##SceneView").c_str(), &selected)) {
                    currComponentSelected = i;
                }
            }
            ImGui::EndCombo();
        }
        if (currComponentSelected != -1 && currComponentSelected < components.size()) {
            ImGui::TextWrapped(components[currComponentSelected].description.c_str());
        }

        if (ImGui::Button("Render")) {
            if (currComponentSelected != -1 && currComponentSelected < components.size()) {
                auto& rs = manager.renderSettingsManager;
                SceneBuilder sceneBuilder{ manager.assetManager.asset, rs.renderSettings, rs.ambientSettings, rs.camera };
                componentManager.exec<RenderComponent>(components[currComponentSelected], sceneBuilder.build());
            }
            else {
                getServer().logger.error("No render component is selected!");
            }
        }

        //if (ImGui::Button("Render")) {
        //    for (int i = 0; i < components.size(); i++) {
        //        //if (components[i].name == "AccPathTracer") {
        //        if (components[i].name == "MetropolisLightTransport") {
        //            currComponentSelected = i;
        //            break;
        //        }
        //    }
        //    //manager.assetManager.StaticimportScene();
        //    if (currComponentSelected != -1 && currComponentSelected < components.size()) {
        //        auto& rs = manager.renderSettingsManager;
        //        SceneBuilder sceneBuilder{manager.assetManager.asset, rs.renderSettings, rs.ambientSettings, rs.camera};
        //        componentManager.exec<RenderComponent>(components[currComponentSelected], sceneBuilder.build());
        //    }
        //    else {
        //        getServer().logger.error("No render component is selected!");
        //    }
        //}
    }

} // namespace NRenderer
