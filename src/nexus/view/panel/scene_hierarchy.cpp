#include "scene_hierarchy.h"

#include "nexus/core/world.h"
#include "nexus/event/client.h"
#include "nexus/event/context_change_event.h"
#include "nexus/event/scene_reset_event.h"
#include "nexus/utility.h"

#include "imgui.h"

constexpr auto BASE_FLAGS = ImGuiTreeNodeFlags_OpenOnArrow |
                            ImGuiTreeNodeFlags_SpanFullWidth |
                            ImGuiTreeNodeFlags_DrawLinesFull;

Nexus::SceneHierarchy::SceneHierarchy()
{
    Client::On<SceneResetEvent>(
        [this](const SceneResetEvent &)
        {
            m_ContextHash = 0;
            auto [stage, _] = World::GetStageWriteAccess();
            m_Range = pxr::UsdPrimRange::PreAndPostVisit(stage->GetPseudoRoot());
        });

    auto [stage, _] = World::GetStageWriteAccess();
    m_Range = pxr::UsdPrimRange::PreAndPostVisit(stage->GetPseudoRoot());
}

void Nexus::SceneHierarchy::draw()
{
    if (ImGui::Begin("Scene Hierarchy"))
    {
        uint8_t treeDepth = 0;
        uint64_t treeStack = 0;

        for (auto it = m_Range.cbegin(); it != m_Range.cend(); it++)
        {
            ImGuiTreeNodeFlags flags = BASE_FLAGS;

            if (m_ContextHash == it->GetPath().GetHash())
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
                    m_ContextHash = flags & ImGuiTreeNodeFlags_Selected ? 0 : it->GetPath().GetHash();
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
                    m_ContextHash = flags & ImGuiTreeNodeFlags_Selected ? 0 : it->GetPath().GetHash();
                    Client::Send<ContextChangeEvent>(it->GetPrim());
                }
                treeStack &= ~bit<uint64_t>(treeDepth++);
            }
        }
    }
    ImGui::End();
}