#include "ui/views/LogView.hpp"

#include "server/Server.hpp"

#undef ERROR

namespace NRenderer
{
    void LogView::drawBeginWindow() {
        ImGui::Begin("Log", nullptr, windowFlag);
    }
    void LogView::draw() {
        bool clear = ImGui::Button("Clear");
        if (clear) {
            getServer().logger.clear();
        }
            
        ImGui::BeginChild("Log Content", {}, true);
        {
            const ImVec4 green{ 0, 1, 0, 1 };
            const ImVec4 red{1, 0, 0, 1};
            const ImVec4 yellow{1, 1, 0, 1};
            auto msgs = getServer().logger.get();
            for (int i=0; i<msgs.nums; i++) {
                auto& msg = msgs.msgs[i];
                switch (msg.type)
                {
                case Logger::LogType::SUCCESS:
                    ImGui::PushStyleColor(ImGuiCol_Text, green);
                    ImGui::TextWrapped(msg.message.c_str());
                    ImGui::PopStyleColor();
                    break;
                case Logger::LogType::WARNING:
                    ImGui::PushStyleColor(ImGuiCol_Text, yellow);
                    ImGui::TextWrapped(msg.message.c_str());
                    ImGui::PopStyleColor();
                    break;
                case Logger::LogType::ERROR:
                    ImGui::PushStyleColor(ImGuiCol_Text, red);
                    ImGui::TextWrapped(msg.message.c_str());
                    ImGui::PopStyleColor();
                    break;
                default:
                    ImGui::TextUnformatted(msg.message.c_str());
                    break;
                }
            } 
        }
        ImGui::EndChild();
    }
    
} // namespace NRenderer
