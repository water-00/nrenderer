#include "ui/views/AssetView.hpp"
#include "importer/SceneImporterFactory.hpp"
#include "templates/MaterialTemplates.hpp"
#include "server/Server.hpp"

#include "imgui_internal.h"

namespace NRenderer
{
    AssetView::AssetView(const Vec2& position, const Vec2& size, UIContext& uiContext, Manager& manager)
        : View              (position, size, uiContext, manager)
        , tempProp          ("property", Property::Wrapper::IntType{})
    {
        currMtlIndex = -1;
        currLightIndex = -1;
        isPropKeyValid = true;
        tempPropTypeIndex = 0;
        tempMaterialItem.material = SharedMaterial(new Material());
    }

    void AssetView::resetTempProp() {
        tempPropTypeIndex = 0;
        isPropKeyValid = true;
        tempProp.key = "property";
        tempProp.type = Property::Type::INT;
        tempProp.valueWrapper.emplace<Property::Wrapper::IntType>(Property::Wrapper::IntType{0});

    }

#pragma region TEMP_PROP_EDITOR
    void AssetView::tempPropEditor() {
        char buf[64];
        strcpy_s<64>(buf, tempProp.key.c_str());
        bool nameChanged = ImGui::InputText("Property Name", buf, 64);
        if (nameChanged)
            tempProp.key = string(buf);
        if (!isPropKeyValid)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, {1, 0, 0, 1});
            ImGui::TextUnformatted("Duplicated Property Name.");
            ImGui::PopStyleColor();
        }
        string types[7] = {
            "Int", "Float", "RGB", "RGBA", "Vec3", "Vec4", "Texture"};
        int beforeIndex = tempPropTypeIndex;
        if (ImGui::BeginCombo("Property Type", types[tempPropTypeIndex].c_str()))
        {
            for (int i = 0; i < 7; i++)
            {
                bool selected = i == tempPropTypeIndex;
                if (ImGui::Selectable(types[i].c_str(), &selected))
                {
                    tempPropTypeIndex = i;
                }
            }
            ImGui::EndCombo();
        }
        if (beforeIndex != tempPropTypeIndex) {
            using W = Property::Wrapper;
            #undef RGB
            #define RESET(__T1__, __T2__)                                          \
                tempProp.valueWrapper.emplace<W::__T1__##Type>(W::__T1__##Type{}); \
                tempProp.type = Property::Type::__T2__
            
            switch (tempPropTypeIndex)
            {
            case 0:
                RESET(Int, INT);
                break;
            case 1:
                RESET(Float, FLOAT);
                break;
            case 2:
                RESET(RGB, RGB);
                break;
            case 3:
                RESET(RGBA, RGBA);
                break;
            case 4:
                RESET(Vec3, VEC3);
                break;
            case 5:
                RESET(Vec4, VEC4);
                break;
            case 6:
                RESET(TextureId, TEXTURE_ID);
                break;
            default:
                break;
            };
            #undef RESET 

        }

        #define SET(__T__, __V__) \
            tempProp.valueWrapper = Property::Wrapper::__T__##Type { __V__ }
        if (tempPropTypeIndex == 0)
        {
            int v = get<0>(tempProp.valueWrapper).value;
            ImGui::InputInt("Value  ##Int", &v, 0, 0);
            SET(Int, v);
        }
        else if (tempPropTypeIndex == 1)
        {
            float v = get<1>(tempProp.valueWrapper).value;
            ImGui::InputFloat("Value  ##Float", &v, 0, 0);
            SET(Float, v);
        }
        else if (tempPropTypeIndex == 2)
        {
            RGB v = get<2>(tempProp.valueWrapper).value;
            ImGui::ColorEdit3("Value  ##RGB", &v[0], ImGuiColorEditFlags_Float);
            SET(RGB, v);
        }
        else if (tempPropTypeIndex == 3)
        {
            RGBA v = get<3>(tempProp.valueWrapper).value;
            ImGui::ColorEdit4("Value  ##RGBA", &v[0], ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_Float);
            SET(RGBA, v);
        }
        else if (tempPropTypeIndex == 4)
        {
            Vec3 v = get<4>(tempProp.valueWrapper).value;
            ImGui::InputFloat3("Value  ##Vec3", &v[0]);
            SET(Vec3, v);
        }
        else if (tempPropTypeIndex == 5)
        {
            Vec4 v = get<5>(tempProp.valueWrapper).value;
            ImGui::InputFloat4("Value  ##RGBA", &v[0]);
            SET(Vec4, v);
        }
        else if (tempPropTypeIndex == 6)
        {
            Handle v = get<6>(tempProp.valueWrapper).value;
            if (ImGui::BeginCombo("Texture##Tex", v.valid() ? manager.assetManager.asset.textureItems[v.index()].name.c_str() : ""))
            {
                for (int i = 0; i < manager.assetManager.asset.textureItems.size(); i++)
                {
                    bool selected = v.valid() ? (i == v.index()) : false;
                    if (ImGui::Selectable(manager.assetManager.asset.textureItems[i].name.c_str(), &selected))
                    {
                        v.setIndex(i);
                    }
                }
                ImGui::EndCombo();
            }
            SET(TextureId, v);
        }
        #undef SET
    }
#pragma endregion TEMP_PROP_EDITOR

    void AssetView::drawBeginWindow() {
        ImGui::Begin("Asset", nullptr, windowFlag | ImGuiWindowFlags_MenuBar);
    }

    void AssetView::draw() {
        menu();
        if (ImGui::BeginTabBar("AssetManager"))  {
            if (ImGui::BeginTabItem("Models")) {
                modelTab();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Materials")) {
                materialTab();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Textures")) {
                textureTab();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Lights")) {
                lightTab();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }

    void AssetView::menu() {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("导入")) {
                if (ImGui::MenuItem("场景/模型")) {
                    manager.assetManager.importScene();
                }
                if (ImGui::MenuItem("纹理")) {
                    manager.assetManager.importTexture();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("重置")) {
                if (ImGui::MenuItem("重置全部")) {
                    manager.assetManager.clearAll();
                }
                ImGui::Separator();
                if (ImGui::MenuItem("重置灯光")) {
                    manager.assetManager.clearLight();
                }
                if (ImGui::MenuItem("重置模型")) {
                    manager.assetManager.clearModel();
                }
                ImGui::EndMenu();
            }
        ImGui::EndMenuBar();
        }
    }

    void AssetView::modelTab() {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        ImGui::Button("Add...(Todo)");
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
        ImGui::Separator();
        ImGui::Columns(2);
        ImGui::BeginChild("ModelSelect");
        {
           const auto flags = ImGuiTableFlags_BordersInnerV 
                | ImGuiTableFlags_BordersInnerH 
                | ImGuiTableFlags_Resizable 
                | ImGuiTableFlags_RowBg
                ;
            if (ImGui::BeginTable("Model/NodeItems##Table", 3, flags)) {
                ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthStretch, 0.15f);
                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("Type");

                auto& mis = manager.assetManager.asset.modelItems;

                for(int i=0; i<mis.size(); i++) {
                    auto& mi = mis[i];
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    bool before_selected = (uiContext.previewModel == i); 
                    bool model_selected = (uiContext.previewModel == i);
                    ImGui::Bullet();
                    if (ImGui::Selectable(("##ModelItemIndex"+to_string(i+1)).c_str(), &model_selected, ImGuiSelectableFlags_SpanAllColumns)) {
                        if (!before_selected) {
                            uiContext.previewModel = i;
                            uiContext.previewMode = UIContext::PreviewMode::PREVIEW_MODEL;
                        }
                        else {
                            uiContext.previewModel = -1;
                        }
                    }
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted((mi.name).c_str());
                    ImGui::TableNextColumn();
                    ImGui::TextDisabled("--");
                    if (model_selected) {
                        auto& nis = manager.assetManager.asset.nodeItems;
                        for (auto nIdx : mi.model->nodes) {
                            auto& ni = nis[nIdx];
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            bool node_selected = (uiContext.previewNode == nIdx);
                            if (ImGui::Selectable((to_string(nIdx+1)+"##NodeItemIndex").c_str(), &node_selected, ImGuiSelectableFlags_SpanAllColumns)) {
                                uiContext.previewNode = nIdx;
                                uiContext.previewMode = UIContext::PreviewMode::PREVIEW_NODE;
                            }
                            ImGui::TableNextColumn();
                            ImGui::TextUnformatted(ni.name.c_str());
                            ImGui::TableNextColumn();
                            string typeStr;
                            auto& n = *ni.node;
                            switch (n.type)
                            {
                            case Node::Type::SPHERE:
                                typeStr = "Sphere";
                                break;
                            case Node::Type::TRIANGLE:
                                typeStr = "Triangle";
                                break;
                            case Node::Type::PLANE:
                                typeStr = "Plane";
                                break;
                            case Node::Type::MESH:
                                typeStr = "Mesh";
                                break;
                            default:
                                break;
                            }
                            ImGui::TextUnformatted(typeStr.c_str());
                        }
                    }
                }

                ImGui::EndTable();
            }
        }
        ImGui::EndChild();
        ImGui::NextColumn();
        ImGui::BeginChild("ModelSetting");
        {
            auto& asset = manager.assetManager.asset;
            auto& mis = asset.modelItems;
            auto& nis = asset.nodeItems;
            if (uiContext.previewMode == UIContext::PreviewMode::PREVIEW_MODEL
                && uiContext.previewModel >= 0 && uiContext.previewModel < mis.size()) {
                auto& mi = mis[uiContext.previewModel];
                ImGui::TextUnformatted("Settings");
                ImGui::TextUnformatted("Name: ");
                ImGui::SameLine();
                ImGui::TextUnformatted(mi.name.c_str());
                ImGui::TextUnformatted("Index: ");
                ImGui::SameLine();
                ImGui::TextUnformatted(to_string(uiContext.previewModel).c_str());
                ImGui::Separator();
                ImGui::DragFloat3(("Translation##ModelSelected"+to_string(uiContext.previewModel)).c_str(), &mi.model->translation.x, 0.5, 0, 0);
                ImGui::DragFloat3(("Scale##ModelSelected"+to_string(uiContext.previewModel)).c_str(), &mi.model->scale.x, 0.05, 0, 0);

            }
            else if (uiContext.previewMode == UIContext::PreviewMode::PREVIEW_NODE
                && uiContext.previewNode >= 0 && uiContext.previewNode < nis.size()) {
                auto& ni = nis[uiContext.previewNode];
                ImGui::TextUnformatted("Settings");
                ImGui::TextUnformatted("Name: ");
                ImGui::SameLine();
                ImGui::TextUnformatted(ni.name.c_str());
                ImGui::TextUnformatted("Index: ");
                ImGui::SameLine();
                ImGui::TextUnformatted(to_string(uiContext.previewNode).c_str());
                auto& mtls = asset.materialItems;
                auto& n = *ni.node;
                auto selectMaterial = [&mtls](Handle& mtlHandle) -> void {
                    bool mtlValid = mtlHandle.valid() && mtlHandle.index() < mtls.size();
                    string comboPreview = mtlValid ? mtls[mtlHandle.index()].name : "";
                    if (ImGui::BeginCombo("Material##SphereNode", comboPreview.c_str())) {
                        for (int i = 0; i < mtls.size(); i++) {
                            bool selected = i == mtlHandle.index();
                            auto& mtl = mtls[i];
                            if (ImGui::Selectable((to_string(i+1)+". "+mtl.name+"##NodeSettings").c_str(), &selected)) {
                                mtlHandle.setIndex(i);
                            }
                        }
                        
                        ImGui::EndCombo();
                    }
                };
                bool change = false;
                if (n.type == Node::Type::SPHERE) {
                    auto& s = *asset.spheres[n.entity];
                    auto& mtlHandle = s.material;
                    selectMaterial(mtlHandle);
               }
                else if (n.type == Node::Type::TRIANGLE) {
                    auto& t = *asset.triangles[n.entity];
                    auto& mtlHandle = t.material;
                    selectMaterial(mtlHandle);      
                }
                else if (n.type == Node::Type::PLANE) {
                    auto& p = *asset.planes[n.entity];
                    auto& mtlHandle = p.material;
                    selectMaterial(mtlHandle); 
                }
                else if (n.type == Node::Type::MESH) {
                    auto& m = *asset.meshes[n.entity];
                    auto& mtlHandle = m.material;
                    selectMaterial(mtlHandle); 
                }
            
                if (change) {
                    asset.updateNodeGlDrawData(ni);
                }
            }
        }
        ImGui::EndChild();
        ImGui::Columns(1);
    }

    void AssetView::materialPropEditor(SharedMaterial spMaterial) {
        using P = Property;
        using T = Property::Type;
        using W = Property::Wrapper;
        #define SET(__T__, __V__) p.valueWrapper = W::__T__##Type{__V__}
        int i = 0;
        for (auto& p : spMaterial->properties) {
            ImGui::PushID(i);
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0, 0.8f, 0.8f));
            if (ImGui::Button(" - ")) {
                spMaterial->removeProperty(p.key);
            }
            ImGui::PopStyleColor(3);
            ImGui::PopID();
            ImGui::SetNextItemWidth(200);
            ImGui::SameLine();
            switch (p.type)
            {
            case T::INT:
            {
                auto v = get<0>(p.valueWrapper).value;
                ImGui::InputInt((p.key+"##PEInt"+to_string(i)).c_str(), &v, 0, 0);
                SET(Int, v);
                break;
            }
            case T::FLOAT:
            {
                auto v = get<1>(p.valueWrapper).value;
                ImGui::InputFloat((p.key+"##PEFloat"+to_string(i)).c_str(), &v, 0, 0);
                SET(Float, v);
                break;
            }
            case T::RGB:
            {
                auto v = get<2>(p.valueWrapper).value;
                ImGui::ColorEdit3((p.key+"##PERGB"+to_string(i)).c_str(), &v[0], ImGuiColorEditFlags_Float);
                SET(RGB, v);
                break;
            }
            case T::RGBA:
            {
                auto v = get<3>(p.valueWrapper).value;
                ImGui::ColorEdit4((p.key+"##PERGBA"+to_string(i)).c_str(), &v[0], ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaPreview);
                SET(RGBA, v);
                break;
            }
            case T::VEC3:
            {
                auto v = get<4>(p.valueWrapper).value;
                ImGui::InputFloat3((p.key+"##PEVec3"+to_string(i)).c_str(), &v[0]);
                SET(Vec3, v);
                break;
            }
            case T::VEC4:
            {
                auto v = get<5>(p.valueWrapper).value;
                ImGui::InputFloat4((p.key+"##PEVec4"+to_string(i)).c_str(), &v[0]);
                SET(Vec4, v);
                break;
            }
            case T::TEXTURE_ID:
            {
                auto v = get<6>(p.valueWrapper).value;
                if (ImGui::BeginCombo((p.key+"##PETex"+to_string(i)).c_str(), v.valid()? manager.assetManager.asset.textureItems[v.index()].name.c_str() : "")) {
                    for (int i=0; i < manager.assetManager.asset.textureItems.size(); i++) {
                        bool selected = v.valid() ? (i == v.index()) : false;
                        if (ImGui::Selectable(manager.assetManager.asset.textureItems[i].name.c_str(), &selected)) {
                            v.setIndex(i);
                        }
                    }
                    ImGui::EndCombo();
                }
                SET(TextureId, v);
                break;
            }
            default:
                break;
            }
            i++;
        };
        #undef SET
    }

    void AssetView::materialTab() {
        if(ImGui::Button("Add...")) ImGui::OpenPopup("Add Material");

        ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if(ImGui::BeginPopupModal("Add Material", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

            char buf[64];
            strcpy_s<64>(buf, tempMaterialItem.name.c_str());
            if (ImGui::InputText("Material Name", buf, 64)) {
                tempMaterialItem.name = string(buf);
            }

            int itid = tempMaterialItem.material->type;
            ImGui::InputInt("Type id", &itid, 0, 0);
            tempMaterialItem.material->type = itid > 0 ? itid : 0;
            ImGui::SameLine();
            makeHelper((
                string("Default Material Templates:") + 
                "\n0 -> " + MaterialTemplates::templates[0].key +
                "\n1 -> " + MaterialTemplates::templates[1].key +
                "\n2 -> " + MaterialTemplates::templates[2].key +
                "\n3 -> " + MaterialTemplates::templates[3].key +
                "\n4 -> " + MaterialTemplates::templates[4].key
            ).c_str());

            if (ImGui::Button("Add property")) {
                ImGui::OpenPopup("Property Editor");
            }
            ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            if (ImGui::BeginPopupModal("Property Editor", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                tempPropEditor();
                if (ImGui::Button("Confirm##Property_Editor")) {
                    if (tempMaterialItem.material->registerProperty(tempProp)) {
                        resetTempProp();
                        ImGui::CloseCurrentPopup();
                    }
                    else {
                        isPropKeyValid = false;
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel##Property_Editor")) {
                    // reset
                    resetTempProp();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            ImGui::SameLine();
            
            if (ImGui::Button("Templates")) {
                ImGui::OpenPopup("Select a template");
            }
            if (ImGui::BeginPopup("Select a template", ImGuiWindowFlags_AlwaysAutoResize)) {
                for (auto &t : MaterialTemplates::templates) {
                    if (ImGui::Selectable(t.second.key.c_str())) {
                        tempMaterialItem.material->properties = t.second.props;
                        tempMaterialItem.material->type = t.second.index;
                        ImGui::CloseCurrentPopup();
                    }
                }
            
                ImGui::EndPopup();
            }

            ImGui::Separator();

            ImGui::BeginChild("Material Props", {400, 400});
            {
                materialPropEditor(tempMaterialItem.material);
            }
            ImGui::EndChild();
            if (ImGui::Button("Add##Add_Material")) {
                manager.assetManager.asset.materialItems.push_back(tempMaterialItem);
                tempMaterialItem = MaterialItem{};
                tempMaterialItem.material = SharedMaterial{new Material{}};
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset##Add_Material")) {
                tempMaterialItem = MaterialItem{};
                tempMaterialItem.material = SharedMaterial{new Material{}};
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel##Add_Material")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::Separator();
        ImGui::Columns(2);
        ImGui::BeginChild("MaterialSelect");
        auto& mtlItems = manager.assetManager.asset.materialItems;
        const auto flags = ImGuiTableFlags_BordersInnerH
                         | ImGuiTableFlags_BordersInnerV
                         | ImGuiTableFlags_RowBg
                         ;
        if (ImGui::BeginTable("MaterialsItems##Table", 2, flags)) {
            ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthStretch, 0.1f);
            ImGui::TableSetupColumn("Name");
            int i=0;
            for (auto& m : mtlItems) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                bool selected = i == currMtlIndex;
                if (ImGui::Selectable((to_string(i+1)+"##MaterialItemIndex").c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns)) {
                    currMtlIndex = i;
                }
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(m.name.c_str());
                i++;
            }
            ImGui::EndTable();
        }
        ImGui::EndChild();
        ImGui::NextColumn();
        ImGui::BeginChild("MaterialSetting");
        {
            if (currMtlIndex < mtlItems.size()) {
                auto& mtlItem = mtlItems[currMtlIndex];
                auto& mtl = *mtlItem.material;
                ImGui::TextUnformatted("Settings:");
                char buf[64];
                strcpy_s<64>(buf, mtlItem.name.c_str());
                if (ImGui::InputText("Name##MTLSelected", buf, 64)) {
                    mtlItem.name = string(buf);
                }
                ImGui::InputInt("Type", (int*)&mtl.type, 0);
                ImGui::SameLine();
                makeHelper((
                    string("Default Material Templates:") + 
                    "\n0 -> " + MaterialTemplates::templates[0].key +
                    "\n1 -> " + MaterialTemplates::templates[1].key +
                    "\n2 -> " + MaterialTemplates::templates[2].key +
                    "\n3 -> " + MaterialTemplates::templates[3].key +
                    "\n4 -> " + MaterialTemplates::templates[4].key
                ).c_str());
                string info;
                auto it = MaterialTemplates::templates.find(mtl.type);
                if (it == MaterialTemplates::templates.end()) {
                    info = "Custom Material";
                }
                else {
                    info = it->second.key;
                }
                ImGui::TextUnformatted(info.c_str());
                if (ImGui::Button("Add Property...")) {
                    ImGui::OpenPopup("Property Editor##2");
                }
                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
                if (ImGui::BeginPopupModal("Property Editor##2", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                    tempPropEditor();
                    if (ImGui::Button("Confirm##Property_Editor")) {
                        if (mtl.registerProperty(tempProp)) {
                            resetTempProp();
                            ImGui::CloseCurrentPopup();
                        }
                        else {
                            isPropKeyValid = false;
                        }
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Cancel##Property_Editor")) {
                        // reset
                        resetTempProp();
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
                ImGui::Separator();
                ImGui::BeginChild("Mtl Props");
                {
                    materialPropEditor(mtlItem.material);
                }
                ImGui::EndChild();
            }
        }
        ImGui::EndChild();
        ImGui::Columns(1);
    }

    void AssetView::textureTab() {
        auto& tItems = manager.assetManager.asset.textureItems;
        int i = 0;
        for (auto &item : tItems) {
            ImGui::Bullet();
            ImGui::SameLine();
            ImGui::BeginGroup();
            {
                float height = 96;
                float width = 128;
                float imgHeight = item.texture->height;
                float imgWidth = item.texture->width;
                ImVec2 v1{0, 0}, v2{1, 1};
                float ratio = width/height;
                if (imgHeight > imgWidth) {
                    float h = 1 / ratio * (imgWidth / imgHeight);
                    v2 = {1, h};
                }
                else {
                    float w = 1 * ratio * (imgHeight / imgWidth);
                    v2 = {w, 1};
                }
                ImGui::Image((void*)item.glId, {width, height}, v1, v2, {1, 1, 1, 1}, {1, 1, 1, 1});
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    float maxWidth = 256;
                    ImVec2 size{};
                    size.x = maxWidth;
                    size.y = maxWidth * imgHeight / imgWidth;
                    ImGui::Image((void*)item.glId, size, {0, 0}, {1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1});
                    ImGui::EndTooltip();
                }
                ImGui::SameLine();
                ImGui::BeginGroup();
                {
                    ImGui::TextUnformatted(string{"Index: " + to_string(i)}.c_str());
                    ImGui::TextUnformatted(string{"Path: " + item.name}.c_str());
                }
                ImGui::EndGroup();
            }
            ImGui::EndGroup();
            i++;
        }
    }

    void AssetView::lightTab() {
        ImGui::Columns(2);
        ImGui::BeginChild("LightSelect");
        const auto flags = ImGuiTableFlags_BordersInnerH
                         | ImGuiTableFlags_BordersInnerV
                         | ImGuiTableFlags_RowBg
                         ;
        auto& lights = manager.assetManager.asset.lightItems;
        if (ImGui::BeginTable("LightItems##Table", 3, flags)) {
            ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthStretch, 0.1f);
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Type");
            int i=0;
            for (auto& l : lights) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                bool selected = i == currLightIndex;
                if (ImGui::Selectable((to_string(i+1)+"##LightItemIndex").c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns)) {
                    currLightIndex = i;
                    uiContext.previewMode = UIContext::PreviewMode::PREVIEW_LIGHT;
                }
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(l.name.c_str());
                ImGui::TableNextColumn();
                string typeStr;
                if (l.light->type == Light::Type::AREA) typeStr = "Area Light";
                else if (l.light->type == Light::Type::DIRECTIONAL) typeStr = "Directional Light";
                else if (l.light->type == Light::Type::POINT) typeStr = "Point Light";
                else if (l.light->type == Light::Type::SPOT) typeStr = "Spot";
                ImGui::TextUnformatted(typeStr.c_str());
                i++;
            }
            ImGui::EndTable();
        }
        ImGui::EndChild();
        ImGui::NextColumn();
        ImGui::BeginChild("LightSetting");
        if (currLightIndex != -1 && currLightIndex < lights.size())
        {
            auto& li = lights[currLightIndex];
            auto& l = *li.light;
            ImGui::TextUnformatted("Settings:");
            using T = Light::Type;
            if (l.type == T::POINT) {
                auto& p = *manager.assetManager.asset.pointLights[l.entity];
                ImGui::DragFloat3(("Intensity##LightSelected"+to_string(currLightIndex)).c_str(), &p.intensity[0], 0.01);
                ImGui::DragFloat3(("Position##LightSelected"+to_string(currLightIndex)).c_str(), &p.position[0], 1);
            }
            else if (l.type == T::AREA) {
                bool dirty = false;
                auto& a = *manager.assetManager.asset.areaLights[l.entity];
                ImGui::DragFloat3(("Intensity##LightSelected"+to_string(currLightIndex)).c_str(), &a.radiance[0], 0.01);
                ImGui::DragFloat3(("Position##LightSelected"+to_string(currLightIndex)).c_str(), &a.position[0], 1);
                dirty |= ImGui::DragFloat3(("U##LightSelected"+to_string(currLightIndex)).c_str(), &a.u[0], 1);
                dirty |= ImGui::DragFloat3(("V##LightSelected"+to_string(currLightIndex)).c_str(), &a.v[0], 1);
                if (dirty) {
                    manager.assetManager.asset.updateLightGlDrawData(li);
                }
            }
            else if (l.type == T::DIRECTIONAL) {
                auto& d = *manager.assetManager.asset.directionalLights[l.entity];
                ImGui::DragFloat3(("Irradiance##LightSelected"+to_string(currLightIndex)).c_str(), &d.irradiance[0], 0.01);
                ImGui::DragFloat3(("Direction##LightSelected"+to_string(currLightIndex)).c_str(), &d.direction[0], 0.1);
            }
            else if (l.type == T::SPOT) {
                auto& s = *manager.assetManager.asset.spotLights[l.entity];
                ImGui::DragFloat3(("Intensity##LightSelected"+to_string(currLightIndex)).c_str(), &s.intensity[0], 0.01);
                ImGui::DragFloat3(("Position##LightSelected"+to_string(currLightIndex)).c_str(), &s.position[0], 1);
                ImGui::DragFloat3(("Direction##LightSelected"+to_string(currLightIndex)).c_str(), &s.direction[0], 0.1);
                float hotSpot = s.hotSpot * 180.f / 3.1415926;
                float fallout = s.fallout * 180.f / 3.1415926;
                ImGui::DragFloat(("HotSpot##LightSelected"+to_string(currLightIndex)).c_str(), &hotSpot, 0.1, 0, 180);
                ImGui::DragFloat(("Fallout##LightSelected"+to_string(currLightIndex)).c_str(), &fallout, 0.1, 0, 180);
                s.hotSpot = hotSpot * 3.1415926 / 180.f;
                s.fallout = fallout * 3.1415926 / 180.f;
            }
        }
        uiContext.previewLight = currLightIndex;
        ImGui::EndChild();
        ImGui::Columns(1);
    }
} // namespace NRenderer
