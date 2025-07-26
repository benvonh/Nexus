#pragma once

#include "nexus/core/world.h"
#include "nexus/view/filedialog.h"

#include "imgui.h"

namespace Nexus
{
    void draw_menu_bar()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New", "Ctrl+N"))
                {
                    FileDialog::Show<FileDialog::Mode::SAVE>(
                        [](std::string path, int)
                        {
                            World::NewStage(path);
                        },
                        FileDialog::USD_FILTER);
                }
                if (ImGui::MenuItem("Open", "Ctrl+O"))
                {
                    FileDialog::Show<FileDialog::Mode::OPEN>(
                        [](std::string path, int)
                        {
                            World::OpenStage(path);
                        },
                        FileDialog::USD_FILTER);
                }
                if (ImGui::MenuItem("Save", "Ctrl+S"))
                {
                    World::SaveStage();
                }
                if (ImGui::MenuItem("Export", "Ctrl+E"))
                {
                    FileDialog::Show<FileDialog::Mode::SAVE>(
                        [](std::string path, int)
                        {
                            World::ExportStage(path);
                        },
                        FileDialog::USD_FILTER);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }
}