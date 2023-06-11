#include "ui/views/View.hpp"

namespace NRenderer
{
    void View::display() {
        drawSetup();
        if (disable) {
            windowFlag |= ImGuiWindowFlags_NoBringToFrontOnFocus;
            windowFlag |= ImGuiWindowFlags_NoInputs;
            windowFlag |= ImGuiWindowFlags_NoFocusOnAppearing;
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.7f);
        }
        if (!resizable) {
            windowFlag |= ImGuiWindowFlags_NoResize;
        }
        if (this->visible) {
            this->drawPosAndSize();
            this->drawBeginWindow();
            this->draw();
            this->drawEndWindow();
        }
        drawFinish();
    }

    void View::drawSetup() {
    }

    void View::drawEndWindow() {
        ImGui::End();
    }

    void View::drawPosAndSize() {
        ImGui::SetNextWindowPos({position.x, position.y}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize({size.x, size.y}, ImGuiCond_FirstUseEver);
    }

    void View::drawFinish() {
        windowFlag = 0;
        if (disable) {
            ImGui::PopStyleVar();
        }
    }

    void View::makeHelper(const char* desc)
    {
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    View::View(const Vec2& position, const Vec2& size, UIContext& uiContext, Manager& manager)
        : position              (position)
        , size                  (size)
        , uiContext             (uiContext)
        , manager               (manager)
        , disable               (false)
        , visible               (true)
        , resizable             (true)
    {}
} // namespace NRenderer
