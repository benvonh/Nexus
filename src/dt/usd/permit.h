#pragma once

#include "dt/utility.h"

#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usdGeom/xformOp.h"

namespace dt
{
    using StagePermit = Permit<pxr::UsdStageRefPtr>;
    using XformOpPermit = Permit<pxr::UsdGeomXformOp>;
}