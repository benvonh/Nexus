#include "logs.h"

#include "dt/logging.h"

#include "imgui.h"

void dt::Logs::draw()
{
    ImGui::Begin("Logs");
    {
        for (const auto &[type, message] : log::Data)
        {
            switch (type)
            {
            case log::Type::Debug:
                ImGui::PushStyleColor(ImGuiCol_Text, {0.5, 0.5, 0.5, 1});
                break;
            case log::Type::Event:
                ImGui::PushStyleColor(ImGuiCol_Text, {0, 1, 0, 1});
                break;
            case log::Type::Alert:
                ImGui::PushStyleColor(ImGuiCol_Text, {1, 1, 0, 1});
                break;
            case log::Type::Error:
                ImGui::PushStyleColor(ImGuiCol_Text, {1, 0, 0, 1});
                break;
            }
            ImGui::TextUnformatted(message.c_str());
        }
        ImGui::PopStyleColor(log::Data.size());
    }
    ImGui::End();
}