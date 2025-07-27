#pragma once

#include "sync.h"

#include "nexus/entity/entity.h"
#include "nexus/logging.h"
#include "nexus/types.h"

#include "pxr/usd/usd/stage.h"

#include "rclcpp/executors/multi_threaded_executor.hpp"

#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>

namespace Nexus
{
    class World final : LOGGER(World)
    {
        friend class Entity;

        using Executor = rclcpp::executors::MultiThreadedExecutor;

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
            return WriteAccess(&s_Stage, &Sync::Mutex);
        }

        [[nodiscard]]
        static auto GetStageReadAccess()
        {
            return ReadAccess(&s_Stage, &Sync::Mutex);
        }

        [[nodiscard]]
        static auto CreateDefaultStage() -> pxr::UsdStageRefPtr;

        static void NewStage(const std::string &path);

        static void OpenStage(const std::string &path);

        static void SaveStage();

        static void ExportStage(const std::string &path);

        static void Spin()
        {
            s_Executor = std::make_unique<Executor>();
            s_Executor->spin();
        }

        template <typename T, typename... Args>
        static void AddEntity(void *key, Args &&...args)
        {
            LOG_EVENT("Adding entity of type <{}> at {}", typeid(T).name(), key);
            static_assert(std::is_base_of_v<Entity, T>, "Must be an Entity type");
            s_Entities[key] = std::make_shared<T>(std::forward<Args>(args)...);
            s_Executor->add_node(s_Entities[key]);
        }

        static void RemoveEntity(void *key)
        {
            LOG_EVENT("Removing entity at {}", key);
            rclcpp::Node::SharedPtr node = s_Entities.at(key);
            s_Executor->remove_node(node);
            s_Entities.erase(key);
        }

    private:
        /* Start time since static initialization */
        static inline const auto START = std::chrono::steady_clock::now();

        /* TODO */
        static inline bool s_Live = false;

        /* TODO */
        static inline double s_Latest = 0;

        static inline pxr::UsdStageRefPtr s_Stage = World::CreateDefaultStage();

        static inline std::unordered_map<void *, std::shared_ptr<Entity>> s_Entities;

        static inline std::unique_ptr<Executor> s_Executor;
    };
}