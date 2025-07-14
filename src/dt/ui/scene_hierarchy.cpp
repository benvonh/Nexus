#include "scene_hierarchy.h"

#include "dt/event/context_change_event.h"
#include "dt/event/scene_reset_event.h"

#include "dt/core/world.h"
#include "dt/logging.h"
#include "dt/utility.h"

#include "imgui.h"

constexpr auto BASE_FLAGS = ImGuiTreeNodeFlags_OpenOnArrow |
                            ImGuiTreeNodeFlags_SpanFullWidth |
                            ImGuiTreeNodeFlags_DrawLinesFull;

dt::SceneHierarchy::SceneHierarchy()
{
    Client::On<SceneResetEvent>(
        [this](const SceneResetEvent &)
        {
            M_ContextHash = 0;
            auto [stage, _] = World::GetStagePermit();
            M_Range = pxr::UsdPrimRange::PreAndPostVisit(stage->GetPseudoRoot());
        });

    auto [stage, _] = World::GetStagePermit();
    M_Range = pxr::UsdPrimRange::PreAndPostVisit(stage->GetPseudoRoot());
}

void dt::SceneHierarchy::draw()
{
    if (ImGui::Begin("Scene Hierarchy"))
    {
        uint8_t treeDepth = 0;
        uint64_t treeStack = 0;

        for (auto it = M_Range.cbegin(); it != M_Range.cend(); it++)
        {
            ImGuiTreeNodeFlags flags = BASE_FLAGS;

            if (M_ContextHash == it->GetPath().GetHash())
            {
                flags |= ImGuiTreeNodeFlags_Selected;
            }

            if (it.IsPostVisit())
            {
                if (treeStack & bit<uint64_t>(--treeDepth))
                {
                    ImGui::TreePop();
                }
            }
            else if (it->GetChildren())
            {
                const char *label = it->GetPath().GetName().c_str();

                const bool open = ImGui::TreeNodeEx((void *)it->GetPath().GetHash(), flags, label);

                if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                {
                    M_ContextHash = flags & ImGuiTreeNodeFlags_Selected ? 0 : it->GetPath().GetHash();
                    Client::Send<ContextChangeEvent>(it->GetPrim());
                }

                if (open)
                {
                    treeStack |= bit<uint64_t>(treeDepth);
                }
                else
                {
                    treeStack &= ~bit<uint64_t>(treeDepth);
                    it.PruneChildren();
                }
                treeDepth++;
            }
            else // leaf node
            {
                flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

                const char *label = it->GetPath().GetName().c_str();

                ImGui::TreeNodeEx((void *)it->GetPath().GetHash(), flags, label);

                if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                {
                    M_ContextHash = flags & ImGuiTreeNodeFlags_Selected ? 0 : it->GetPath().GetHash();
                    Client::Send<ContextChangeEvent>(it->GetPrim());
                }
                treeStack &= ~bit<uint64_t>(treeDepth++);
            }
        }
    }
    ImGui::End();
}