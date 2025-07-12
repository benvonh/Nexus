#pragma once

#include "dt/core/permit.h"
#include "dt/event/client.h"

#include "pxr/usd/usd/stage.h"

#include <chrono>
#include <mutex>
#include <string>

namespace dt
{
    class World : Client
    {
    public:
        World() = delete;
        ~World() = delete;

        static inline const auto START = std::chrono::steady_clock::now();

        [[nodiscard]]
        static auto GetTime() noexcept
        {
            using namespace std::chrono;
            return duration<double>(steady_clock::now() - START).count();
        }

        [[nodiscard]]
        static auto GetStagePermit()
        {
            return StagePermit(S_Stage, &S_Mutex);
        }

        [[nodiscard]]
        static auto DefaultStage() -> pxr::UsdStageRefPtr;

        static void NewStage(const std::string &path);

        static void OpenStage(const std::string &path);

        static void SaveStage();

        static void ExportStage(const std::string &path);

    private:
        static inline std::mutex S_Mutex;
        static inline pxr::UsdStageRefPtr S_Stage = World::DefaultStage();
    };
}