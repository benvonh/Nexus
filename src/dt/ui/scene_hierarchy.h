#pragma once

#include "dt/logging.h"
#include "dt/core/world.h"

#include "pxr/usd/usd/primRange.h"

#include "imgui.h"

namespace dt
{
    void draw_scene_hierarchy()
    {
        if (ImGui::Begin("Scene Hierarchy"))
        {
            uint8_t treeDepth = 0;
            uint64_t treeStack = 0;

            auto [stage, _] = World::GetStagePermit();

            const auto range = pxr::UsdPrimRange::PreAndPostVisit(stage->GetPseudoRoot());

            for (auto it = range.cbegin(); it != range.cend(); it++)
            {
                if (it.IsPostVisit())
                {
                    if (treeStack & (1ULL << --treeDepth))
                    {
                        ImGui::TreePop();
                    }
                }
                else
                {
                    if (ImGui::TreeNode(it->GetPath().GetText()))
                    {
                        treeStack |= (1ULL << treeDepth);
                    }
                    else
                    {
                        treeStack &= ~(1ULL << treeDepth);
                        it.PruneChildren();
                    }
                    treeDepth++;
                }
            }
        }
        ImGui::End();
    }
}