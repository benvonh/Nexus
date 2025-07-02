#pragma once
#include "pxr/usd/usd/stage.h"
#include <mutex>

namespace dt
{
    ///
    ///
    ///
    /// @brief
    /// @tparam T
    ///
    ///
    ///
    template <typename T>
    struct Permit
    {
        Permit(std::mutex *m, T &r) : Guard(*m), Ref(r) {}

        std::lock_guard<std::mutex> Guard;

        T &Ref;
    };

    ///
    ///
    ///
    /// Blah
    ///
    ///
    ///
    template <>
    struct Permit<pxr::UsdStageRefPtr>
    {
        Permit(std::mutex *m, pxr::UsdStageRefPtr &r) : Guard(*m), Stage(r) {}

        std::lock_guard<std::mutex> Guard;

        pxr::UsdStageRefPtr &Stage;
    };

    using StagePermit = Permit<pxr::UsdStageRefPtr>;
}