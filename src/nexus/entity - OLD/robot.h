#pragma once

#include "nexus/usd/world.h"
#include "nexus/usd/permit.h"

#include "pxr/usd/usdGeom/xformOp.h"

#include <string>
#include <unordered_map>

namespace Nexus
{
    using LinkMap = std::unordered_map<std::string, pxr::UsdGeomXformOp>;
    using LinkMapPermit = Permit<LinkMap>;

    class Robot
    {
    public:
        void reset();

        void from_urdf(const std::string &path);

        [[nodiscard]] auto get_xform_ops_permit()
        {
            return LinkMapPermit(_XformOps, &World::__Mutex);
        }

    private:
        LinkMap _XformOps;
    };
}