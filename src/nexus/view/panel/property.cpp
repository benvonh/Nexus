#include "property.h"

#include "nexus/core/world.h"
#include "nexus/event/client.h"
#include "nexus/event/context_change_event.h"
#include "nexus/event/scene_reset_event.h"

#include "pxr/base/gf/matrix4d.h"
#include "pxr/usd/sdf/types.h"
#include "pxr/usd/sdf/valueTypeName.h"
#include "pxr/usd/usd/attribute.h"
#include "pxr/usd/usd/prim.h"

#include "imgui.h"

Nexus::Property::Property()
{
    Client::On<SceneResetEvent>(
        [this](const SceneResetEvent &)
        {
            m_ContextPrim = pxr::UsdPrim();
        });

    Client::On<ContextChangeEvent>(
        [this](const ContextChangeEvent &e)
        {
            m_ContextPrim = e.Prim;
        });
}

void Nexus::Property::draw()
{
    if (ImGui::Begin("Property"))
    {
        if (m_ContextPrim.IsValid())
        {
            auto [stage, lock] = World::GetStageWriteAccess();

            for (const auto &attribute : m_ContextPrim.GetAttributes())
            {
                const auto typeName = attribute.GetTypeName();

                // See pxr/usd/sdf/types.h
                if (typeName == pxr::SdfValueTypeNames->Matrix4d)
                {
                    pxr::GfMatrix4d matrix;

                    if (attribute.Get(&matrix, World::GetTime()))
                    {
                        ImGui::SeparatorText("Matrix4d");
                        ImGui::InputFloat4("Matrix Row 0", (float *)matrix.data());
                        ImGui::InputFloat4("Matrix Row 1", (float *)matrix.data() + 4);
                        ImGui::InputFloat4("Matrix Row 2", (float *)matrix.data() + 8);
                        ImGui::InputFloat4("Matrix Row 3", (float *)matrix.data() + 12);
                    }
                }
                // TODO
                // else if (typeName == pxr::SdfValueTypeNames->Quatd)
                // {
                //     ImGui::Text("Type: Quatd");
                // }
                // else if (typeName == pxr::SdfValueTypeNames->Quatf)
                // {
                //     ImGui::Text("Type: Quatf");
                // }
                // else if (typeName == pxr::SdfValueTypeNames->Quath)
                // {
                //     ImGui::Text("Type: Quath");
                // }
                else
                {
                    ImGui::SeparatorText(typeName.GetAsToken().GetText());
                }
            }
        }
        else
        {
            ImGui::TextUnformatted("No selected Prim");
        }
    }
    ImGui::End();
}