#pragma once

#include "nexus/entity/entity.h"
#include "nexus/logging.h"

#include "pxr/usd/usdGeom/xformOp.h"

#include "rclcpp/timer.hpp"
#include "sensor_msgs/msg/joint_state.hpp"

#include "tf2_ros/buffer.h"
#include "tf2_ros/transform_listener.h"

#include <string>
#include <unordered_map>

namespace Nexus
{
    class Robot : public Entity, LOGGER(Robot)
    {
        struct Data
            : public Entity::Data,
              std::unordered_map<std::string, pxr::UsdGeomXformOp>
        {
        };

        using Timer = rclcpp::TimerBase;
        using TF_Buffer = tf2_ros::Buffer;
        using TF_Listener = tf2_ros::TransformListener;

    public:
        Robot(const std::string &urdf_path);

    protected:
        Entity::Data *_create_data() override;

    private:
        const std::string c_URDF_Path;

        std::shared_ptr<Timer> m_Timer;
        std::unique_ptr<TF_Buffer> m_Buffer;
        std::shared_ptr<TF_Listener> m_Listener;
    };
}