#include "browser.h"

#include "dt/usd/world.h"
#include "dt/logging.h"

#include "pxr/usd/usd/primRange.h"

#include "imgui.h"

void dt::Browser::draw()
{
    ImGui::Begin("Browser");
    {
        size_t treeDepth = 0;
        uint64_t treeStack = 0;

        auto permit = World::GetStagePermit();

        const auto range = pxr::UsdPrimRange::PreAndPostVisit(permit.Stage->GetPseudoRoot());

        for (auto iterator = range.cbegin(); iterator != range.cend(); iterator++)
        {
            if (iterator.IsPostVisit())
            {
                if (treeStack & (1ULL << --treeDepth))
                {
                    ImGui::TreePop();
                }
            }
            else
            {
                if (ImGui::TreeNode(iterator->GetPath().GetText()))
                {
                    treeStack |= (1ULL << treeDepth);
                }
                else
                {
                    treeStack &= ~(1ULL << treeDepth);
                    iterator.PruneChildren();
                }
                treeDepth++;
            }
        }
    }
    ImGui::End();
}