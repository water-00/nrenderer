#include "ui/views/ScreenView.hpp"
#include "server/Server.hpp"

#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace NRenderer
{

#pragma region __SHADER_SOURCE__
    const char* nodeVShaderSource = 
        "#version 330 core                                                  \n"
        "layout (location = 0) in vec3 aPos;                                \n"
        "uniform mat4 model;                                                \n"
        "uniform mat4 view;                                                 \n"
        "uniform mat4 projection;                                           \n"
        "void main()                                                        \n"
        "{                                                                  \n"
        "   gl_Position = projection*view*model*vec4(aPos.x, aPos.y, aPos.z, 1.0);      \n"
        "}                                                                  \n"
        ;
    const char* nodeFShaderSource = 
        "#version 330 core                                                  \n"
        "out vec4 FragColor;                                                \n"
        "uniform vec4 drawColor;                                            \n"
        "void main()                                                        \n"
        "{                                                                  \n"
        "   FragColor = drawColor;                                          \n"
        "}                                                                  \n"
        ;
    const char* lightVShaderSource = 
        "#version 330 core                                                  \n"
        "layout (location = 0) in vec3 aPos;                                \n"
        "uniform mat4 model;                                                \n"
        "uniform mat4 view;                                                 \n"
        "uniform mat4 projection;                                           \n"
        "void main()                                                        \n"
        "{                                                                  \n"
        "   mat4 vm = view;                                                 \n"
        "   gl_Position = projection*vm*model*vec4(aPos, 1.0);              \n"
        "}                                                                  \n"
        ;
    const char* lightFShaderSource = 
        "#version 330 core                                                  \n"
        "out vec4 FragColor;                                                \n"
        "uniform vec4 drawColor;                                            \n"
        "void main()                                                        \n"
        "{                                                                  \n"
        "   FragColor = drawColor;                                          \n"
        "}                                                                  \n"
        ;
#pragma endregion __SHADER_SOURCE__

    ScreenView::ScreenView(const Vec2& position, const Vec2& size, UIContext& uiContext, Manager& manager)
        : View                      (position, size, uiContext, manager)
        , renderResult              (0)
        , previewResult             (0)
        , viewType                  (ViewType::PREVIEW)
        , shrinkLevel               (0)
        , previewCoordinateType     (CoordinateType::LEFT_HANDED)
        , nodeShader                (nodeVShaderSource, nodeFShaderSource)
        , lightShader               (lightVShaderSource, lightFShaderSource)
    {
        // FBO
        glFBO = 0;
        glRBO = 0;

        prePreviewHeight = 0;
        prePreviewWidth = 0;

        previewHeight = 0;
        previewWidth = 0;
    }

    bool ScreenView::isPreviewSizeChange() {
        bool f = (previewWidth != prePreviewWidth || previewHeight != prePreviewHeight);
        return f;
    }

    void ScreenView::updatePreviewSize(unsigned int x, unsigned int y) {
        previewHeight = y;
        previewWidth = x;
    }

    void ScreenView::updatePrePreviewSize() {
        prePreviewWidth = previewWidth;
        prePreviewHeight = previewHeight;
    }

    void ScreenView::drawBeginWindow() {
        ImGui::Begin("Result", nullptr, windowFlag | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    }

    float ScreenView::getShrinkNum() {
        return 1.f - float(shrinkLevel)*0.125;
    }

    void ScreenView::draw() {
        auto size = ImGui::GetWindowSize();
        size.y -= 65;
        size.x -= 10;
        if (getServer().screen.isUpdated()) viewType = ViewType::RESULT;
        ImGui::BeginChild("Screen Content", size, false, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoMove);
        {
            if (viewType == ViewType::PREVIEW) {
                preview();
            }
            else if (viewType == ViewType::RESULT) {
                result();
            }
        }
        ImGui::EndChild();
        ImGui::Separator();
        if (viewType == ViewType::PREVIEW) {
            if (ImGui::Button("Preview", {80, 22})) viewType = ViewType::RESULT;
        }
        else if (viewType == ViewType::RESULT) {
            if (ImGui::Button("Result ", {80, 22})) viewType = ViewType::PREVIEW;
        }

        ImGui::SameLine();
        const string shrinkLevelsLabel[8] = {
            "100.0%", "87.5%", "75.0%", "62.5%", "50.0%", "37.5%", "25.0%", "12.5%"
        };
        ImGui::SetNextItemWidth(80);
        if (ImGui::BeginCombo("##ShrinkLevel", shrinkLevelsLabel[shrinkLevel].c_str())) {
            for (int i=0; i < 8; i++) {
                bool selected = i == shrinkLevel;
                if (ImGui::Selectable(shrinkLevelsLabel[i].c_str(), &selected)) {
                    shrinkLevel = i;
                }
            }
            ImGui::EndCombo();
        }
        if (viewType == ViewType::PREVIEW) {
            ImGui::SameLine();
            if (previewCoordinateType == CoordinateType::LEFT_HANDED) {
                if (ImGui::Button("Left-handed", {100, 22})) previewCoordinateType = CoordinateType::RIGHT_HANDED;
            }
            else {
                if (ImGui::Button("Right-handed", {100, 22})) previewCoordinateType = CoordinateType::LEFT_HANDED;
            }
        }
    }

    void ScreenView::preview() {
        auto ux = manager.renderSettingsManager.renderSettings.width;
        auto uy = manager.renderSettingsManager.renderSettings.height;
        float x = ux;
        float y = uy;
        x *= getShrinkNum();
        y *= getShrinkNum();
        updatePreviewSize(x, y);
        if (isPreviewSizeChange()) {
            genFB();
        }
        this->align({x, y});
        drawGlPreview();
        ImVec2 bv = {1, 1}, ev = {0, 0};
        if (previewCoordinateType == CoordinateType::RIGHT_HANDED) {
            bv = {0, 1};
            ev = {1, 0};
        }
        ImGui::Image((void*)previewResult, { x, y }, bv, ev);
        updatePrePreviewSize();
    }

    void ScreenView::result() {
        Vec2 rs = {getServer().screen.getWidth(), getServer().screen.getHeight()};
        if (getServer().screen.isUpdated()) {
            if (renderResult != 0) GlImage::deleteImage(renderResult);
            auto pixels = getServer().screen.getPixels();
            renderResult = GlImage::loadImage(pixels, rs);
        }
        rs *= getShrinkNum();
        this->align(rs);
        ImGui::Image((void*)(intptr_t)renderResult, { rs.x, rs.y }, { 0, 0 }, { 1, 1 });
    }

    void ScreenView::align(const Vec2& size) {
        auto [x, y] = ImGui::GetWindowSize();
        if (size.x < x) {
            ImGui::SetCursorPosX((x-size.x)/2);
        }
        if (size.y < y) {
            ImGui::SetCursorPosY((y-size.y)/2);
        }
    }

    void ScreenView::genFB() {
        auto& TX = previewResult;
        if (TX != 0) {
            glDeleteTextures(1, &TX);
            TX = 0;
        }
        if (glFBO != 0) {
            glDeleteFramebuffers(1, &glFBO);
            glFBO = 0;
        }
        if (glRBO != 0) {
            glDeleteRenderbuffers(1, &glRBO);
            glRBO = 0;
        }
        
        glGenTextures(1, &TX);
        glBindTexture(GL_TEXTURE_2D, TX);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, previewWidth, previewHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glGenRenderbuffers(1, &glRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, glRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, previewWidth, previewHeight);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glGenFramebuffers(1, &glFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, glFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, previewResult, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, glRBO);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void ScreenView::drawGlPreview() {
        glBindFramebuffer(GL_FRAMEBUFFER, glFBO);
        nodeShader.use();

        glViewport(0, 0, previewWidth, previewHeight);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        auto& camera = manager.renderSettingsManager.camera;

        Mat4x4 projection = glm::perspective(glm::radians(camera.fov), camera.aspect, 0.1f, 10000000.0f);
        Mat4x4 view = glm::lookAt(camera.position, camera.lookAt, camera.up);

        nodeShader.setMat4x4("projection", projection);
        nodeShader.setMat4x4("view", view);

        const Vec4 commonColor{1, 1, 0, 1};
        const Vec4 selectedColor{1, 0.5, 0.5, 1};

        auto& asset = manager.assetManager.asset;
        for (int i=0; i<asset.nodeItems.size(); i++) {
            auto& ni = asset.nodeItems[i];
            if (uiContext.previewMode == UIContext::PreviewMode::PREVIEW_MODEL) {
                if (uiContext.previewModel != ni.node->model) {
                    nodeShader.setVec4("drawColor", commonColor);
                    previewNode(ni);
                }
            }
            else if (uiContext.previewMode == UIContext::PreviewMode::PREVIEW_NODE) {
                if (uiContext.previewNode != i) {    
                    nodeShader.setVec4("drawColor", commonColor);
                    previewNode(ni);
                }
            }
            else {
                nodeShader.setVec4("drawColor", commonColor);
                previewNode(ni);
            }
        }
        if (uiContext.previewMode == UIContext::PreviewMode::PREVIEW_MODEL) {
            if (uiContext.previewModel != -1 && uiContext.previewModel < asset.modelItems.size()) {
                auto& m = *asset.modelItems[uiContext.previewModel].model;
                for (auto& nIdx : m.nodes) {
                    nodeShader.setVec4("drawColor", selectedColor);
                    previewNode(asset.nodeItems[nIdx]);
                }
            }
        }
        else if (uiContext.previewMode == UIContext::PreviewMode::PREVIEW_NODE) {
            if (uiContext.previewNode != -1 && uiContext.previewNode < asset.nodeItems.size()) {
                nodeShader.setVec4("drawColor", selectedColor);
                previewNode(asset.nodeItems[uiContext.previewNode]);
            }
        }

        lightShader.use();
        lightShader.setMat4x4("projection", projection);
        lightShader.setMat4x4("view", view);

        const Vec4 commonLightColor{1, 1, 1, 1};
        const Vec4 selectedLightColor{0.5, 0.5, 1, 1};

        for (int i=0; i < asset.lightItems.size(); i++) {
            auto& l = asset.lightItems[i];
            if (uiContext.previewMode == UIContext::PreviewMode::PREVIEW_LIGHT) {
                if (i != uiContext.previewLight) {
                    lightShader.setVec4("drawColor", commonLightColor);
                    previewLight(l);
                }
            }
            else {
                lightShader.setVec4("drawColor", commonLightColor);
                previewLight(l);
            }
        }
        if (uiContext.previewMode == UIContext::PreviewMode::PREVIEW_LIGHT) {
            if (uiContext.previewLight != -1 && uiContext.previewLight < asset.lightItems.size()) {
                lightShader.setVec4("drawColor", selectedLightColor);
                previewLight(asset.lightItems[uiContext.previewLight]);
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void ScreenView::previewNode(const NodeItem& n) {
        auto& camera = manager.renderSettingsManager.camera;
        auto& asset = manager.assetManager.asset;
        glBindVertexArray(n.glVAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        Mat4x4 modelMat{1};
        auto& model = *asset.modelItems[n.node->model].model;
        if (n.node->type == Node::Type::TRIANGLE) {
            modelMat = glm::translate(modelMat, model.translation);
            modelMat = glm::scale(modelMat, model.scale);
            nodeShader.setMat4x4("model", modelMat);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        else if (n.node->type == Node::Type::PLANE) {
            modelMat = glm::translate(modelMat, model.translation);
            modelMat = glm::scale(modelMat, model.scale);
            nodeShader.setMat4x4("model", modelMat);
            glDrawArrays(GL_LINE_LOOP, 0, 4);
        }
        else if (n.node->type == Node::Type::SPHERE) {
            const Vec3 norm{0, 0, -1};
            Vec3 pos = asset.spheres[n.node->entity]->position + model.translation;
            Vec3 dir = camera.position - camera.lookAt;
            dir = -glm::normalize(dir);
            float cos_theta = glm::dot(dir, norm);
            modelMat = glm::translate(modelMat, pos);
            modelMat = glm::scale(modelMat, model.scale);
            if (dir != norm && dir != -norm) {
                Vec3 axis = glm::cross(dir, norm);
                float angle = -acos(cos_theta);
                modelMat = glm::rotate(modelMat,angle, axis);
            }
            nodeShader.setMat4x4("model", modelMat);
            glDrawArrays(GL_LINE_STRIP, 0, n.externalDrawData->positions.size());
        }
        else if (n.node->type == Node::Type::MESH) {
            modelMat = glm::translate(modelMat, model.translation);
            modelMat = glm::scale(modelMat, model.scale);
            nodeShader.setMat4x4("model", modelMat);
            auto& m = *asset.meshes[n.node->entity];
            glDrawElements(GL_TRIANGLES, m.positionIndices.size(), GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(0);
    }

    void ScreenView::previewLight(const LightItem& l) {
        auto& camera = manager.renderSettingsManager.camera;
        auto& asset = manager.assetManager.asset;
        glBindVertexArray(l.glVAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        Mat4x4 model{1};
        if (l.light->type == Light::Type::AREA) {
            model = glm::translate(model, asset.areaLights[l.light->entity]->position);
            lightShader.setMat4x4("model", model);
            glDrawArrays(GL_LINE_LOOP, 0, 4);
        }
        else if (l.light->type == Light::Type::POINT) {
            const Vec3 n{0, 0, -1};
            Vec3 dir = camera.position - camera.lookAt;
            float dis = glm::length(camera.position - asset.pointLights[l.light->entity]->position);
            float delta = 1/tan(glm::radians(camera.fov/2));
            float scale = (dis + delta) / delta;
            dir = -glm::normalize(dir);
            float cos_theta = glm::dot(dir, n);
            model = glm::translate(model, asset.pointLights[l.light->entity]->position);
            model = glm::scale(model, Vec3{scale});
            if (dir != n && dir != -n) {
                Vec3 axis = glm::cross(dir, n);
                float angle = -acos(cos_theta);
                model = glm::rotate(model, angle, axis);
            }
            lightShader.setMat4x4("model", model);
            glDrawArrays(GL_LINES, 0, 16);
        }
        glBindVertexArray(0);
    }

} // namespace NRenderer
