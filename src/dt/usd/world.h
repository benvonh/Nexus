#pragma once

#include "dt/usd/permit.h"

#include "pxr/usd/usd/stage.h"

#include <chrono>
#include <mutex>
#include <string>

namespace dt
{
    class World
    {
    public:
        World() = delete;
        ~World() = delete;

        static inline const auto START = std::chrono::steady_clock::now();

        [[nodiscard]] static double GetTime() noexcept
        {
            using namespace std::chrono;
            return duration<double>(steady_clock::now() - START).count();
        }

        [[nodiscard]] static StagePermit GetStagePermit() noexcept
        {
            return StagePermit(&__Mutex, __Stage);
        }

        static void NewStage(const std::string &path);
        static void OpenStage(const std::string &path);
        static void SaveStage();
        static void ExportStage(const std::string &path);

    private:
        [[nodiscard]] static pxr::UsdStageRefPtr __DefaultStage();

        static inline std::mutex __Mutex;
        static inline pxr::UsdStageRefPtr __Stage = __DefaultStage();
    };
}