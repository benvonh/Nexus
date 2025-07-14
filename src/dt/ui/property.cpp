#include "property.h"

#include "dt/core/world.h"
#include "dt/event/context_change_event.h"
#include "dt/event/scene_reset_event.h"

#include "pxr/usd/sdf/types.h"
#include "pxr/usd/sdf/valueTypeName.h"
#include "pxr/base/gf/matrix4d.h"

#include "imgui.h"

dt::Property::Property()
{
    Client::On<SceneResetEvent>(
        [this](const SceneResetEvent &)
        {
            _Context_prim = pxr::UsdPrim();
        });

    Client::On<ContextChangeEvent>(
        [this](const ContextChangeEvent &e)
        {
            _Context_prim = e.Prim;
        });
}

void dt::Property::draw()
{
    if (ImGui::Begin("Property"))
    {
        if (_Context_prim.IsValid())
        {
            auto [stage, _] = World::GetStagePermit();

            for (const auto &attribute : _Context_prim.GetAttributes())
            {
                const auto typeName = attribute.GetTypeName();

                if (typeName == pxr::SdfValueTypeNames->Matrix4d)
                {
                    pxr::GfMatrix4d matrix;

                    if (attribute.Get(&matrix, World::GetTime()))
                    {
                        ImGui::Text("Type: Matrix4d");
                        ImGui::Text("Values:");
                        ImGui::Text("%f %f %f %f", matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3]);
                        ImGui::Text("%f %f %f %f", matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3]);
                        ImGui::Text("%f %f %f %f", matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]);
                        ImGui::Text("%f %f %f %f", matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]);
                    }
                }
                else if (typeName == pxr::SdfValueTypeNames->Quatd)
                {
                    ImGui::Text("Type: Quatd");
                }
                else if (typeName == pxr::SdfValueTypeNames->Quatf)
                {
                    ImGui::Text("Type: Quatf");
                }
                else if (typeName == pxr::SdfValueTypeNames->Quath)
                {
                    ImGui::Text("Type: Quath");
                }
                else
                {
                    ImGui::Text("Type: %s", typeName.GetAsToken().GetText());
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