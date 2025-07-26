#pragma once

#include "nexus/logging.h"
#include "nexus/types.h"

#include "pxr/usd/usd/stage.h"

#include <chrono>
#include <shared_mutex>
#include <string>
#include <vector>

namespace Nexus
{
    class World : Logger<"World">
    {
    public:
        World() = delete;
        ~World() = delete;

        [[nodiscard]]
        static auto GetTime() noexcept
        {
            using namespace std::chrono;
            return duration<double>(steady_clock::now() - START).count();
        }

        [[nodiscard]]
        static auto GetStageWriteAccess()
        {
            return WriteAccess(&s_Stage, &s_Mutex);
        }

        [[nodiscard]]
        static auto GetStageReadAccess()
        {
            return ReadAccess(&s_Stage, &s_Mutex);
        }

        [[nodiscard]]
        static auto CreateDefaultStage() -> pxr::UsdStageRefPtr;

        static void NewStage(const std::string &path);

        static void OpenStage(const std::string &path);

        static void SaveStage();

        static void ExportStage(const std::string &path);

        static inline const auto START = std::chrono::steady_clock::now();

    private:
        static inline bool s_Live = false;

        static inline double s_Latest = 0;

        static inline std::shared_mutex s_Mutex;

        // static inline std::vector<Entity> S_Entities;

        static inline pxr::UsdStageRefPtr s_Stage = World::CreateDefaultStage();
    };
}