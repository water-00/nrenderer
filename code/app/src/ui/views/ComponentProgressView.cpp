#include "ui/views/ComponentProgressView.hpp"

namespace NRenderer
{
    void ComponentProgressView::drawBeginWindow() {}
    void ComponentProgressView::drawEndWindow() {}
    void ComponentProgressView::drawSetup() {}
    void ComponentProgressView::drawFinish() {}
    void ComponentProgressView::drawPosAndSize() {
        ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize({size.x, size.y}, ImGuiCond_FirstUseEver);
    }
    void ComponentProgressView::draw() {
        auto& componentManager = manager.componentManager;
        if (componentManager.getState() == ComponentManager::State::IDLING) return;
        auto activeComponentInfo = componentManager.getActiveComponentInfo();
        ImGui::OpenPopup("Executing Component");
        if (ImGui::BeginPopupModal("Executing Component", nullptr, 0)) {
            if (componentManager.getState() == ComponentManager::State::RUNNING) {
                uiContext.state = UIContext::State::HOVER_COMPONENT_PROGRESS;
                ImGui::TextUnformatted(("正在执行: " + activeComponentInfo.id).c_str());
            }
            else if (componentManager.getState() == ComponentManager::State::READY) {
                uiContext.state = UIContext::State::HOVER_COMPONENT_PROGRESS;
                ImGui::TextUnformatted(("准备执行: " + activeComponentInfo.id).c_str());
            }
            else if (componentManager.getState() == ComponentManager::State::FINISH) {
                auto execTime = componentManager.getLastExecTime();
                componentManager.finish();
                string logInfo{};
                logInfo = activeComponentInfo.id + "执行完毕. Time: " + to_string(execTime.count()) + "s";
                getServer().logger.success(logInfo);
                uiContext.state = UIContext::State::NORMAL;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
}