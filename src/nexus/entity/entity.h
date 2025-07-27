#pragma once

#include "nexus/core/sync.h"
#include "nexus/types.h"

#include "rclcpp/node.hpp"

#include <memory>
#include <string>

namespace Nexus
{
    class Entity : public rclcpp::Node
    {
    protected:
        struct Data
        {
            virtual ~Data() = default;
        };

    public:
        Entity(const std::string &name) : Node(name), m_Data(_create_data()) {}

        virtual ~Entity() = default;

    protected:
        [[nodiscard]]
        virtual Data *_create_data() = 0;

        template <typename T>
        auto _get_write_access()
        {
            static_assert(std::is_base_of_v<Data, T>, "Must be an Entity::Data type");
            return WriteAccess(static_cast<T *>(m_Data.get()), &Sync::Mutex);
        }

        template <typename T>
        auto _get_read_access()
        {
            static_assert(std::is_base_of_v<Data, T>, "Must be an Entity::Data type");
            return ReadAccess(static_cast<T *>(m_Data.get()), &Sync::Mutex);
        }

    private:
        std::unique_ptr<Data> m_Data;
    };
}