#include "browser.h"

#include "dt/usd/world.h"

#include "pxr/usd/usd/primRange.h"

#include "imgui.h"

void dt::Browser::draw()
{
    ImGui::Begin("Browser");
    {
        auto permit = World::GetStagePermit();

        for (auto it : permit.Stage->Traverse())
        {
            ImGui::Text("%s (%s)", it.GetPath().GetText(), it.GetTypeName().GetText());
        }
    }
    ImGui::End();
}