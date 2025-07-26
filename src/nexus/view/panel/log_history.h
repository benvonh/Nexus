#pragma once

#include "nexus/logging.h"

#include "imgui.h"

namespace Nexus
{
    void draw_log_history()
    {
        if (ImGui::Begin("Log History"))
        {
            for (const auto &[text, type] : Log::Book::Get())
            {
                switch (type)
                {
                case Log::Type::BASIC:
                    ImGui::PushStyleColor(ImGuiCol_Text, {0.9, 0.9, 0.9, 1});
                    break;
                case Log::Type::EVENT:
                    ImGui::PushStyleColor(ImGuiCol_Text, {0, 1, 0, 1});
                    break;
                case Log::Type::ALERT:
                    ImGui::PushStyleColor(ImGuiCol_Text, {1, 1, 0, 1});
                    break;
                case Log::Type::ERROR:
                    ImGui::PushStyleColor(ImGuiCol_Text, {1, 0, 0, 1});
                    break;
                }
                ImGui::TextUnformatted(text);
            }
            ImGui::PopStyleColor(Log::Book::Size());
        }
        ImGui::End();
    }
}