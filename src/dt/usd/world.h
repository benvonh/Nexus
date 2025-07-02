#pragma once
#include "dt/usd/permit.h"
#include "pxr/usd/usd/stage.h"
#include <chrono>
#include <mutex>

namespace dt
{
    class World
    {
    public:
        World() = delete;
        ~World() = delete;

        static inline const auto START = std::chrono::steady_clock::now();

        static double get_time() noexcept
        {
            using namespace std::chrono;
            return duration<double>(steady_clock::now() - START).count();
        }

        static StagePermit get_stage_permit() noexcept
        {
            return StagePermit(&_Mutex, _Stage);
        }

    private:
        static pxr::UsdStageRefPtr default_stage();

        static inline std::mutex _Mutex;
        static inline pxr::UsdStageRefPtr _Stage = World::default_stage();
    };
}