#pragma once

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "tf2_ros/transform_listener.h"
#include "tf2_ros/buffer.h"

namespace dt
{
  class Bridge final : public rclcpp::Node
  {
  public:
    Bridge() : rclcpp::Node("digital_twin")
    {
      this->buffer = std::make_unique<tf2_ros::Buffer>(this->get_clock());
      this->listener = std::make_shared<tf2_ros::TransformListener>(*this->buffer);
      this->timer = this->create_wall_timer(std::chrono::seconds(1), [this]()
                                            {
        try
        {
          auto t = this->buffer->lookupTransform("odom", "head", tf2::TimePointZero);
          RCLCPP_INFO(this->get_logger(), "Transform: %f %f %f", t.transform.translation.x, t.transform.translation.y, t.transform.translation.z);
        }
        catch (const tf2::TransformException &ex)
        {
          // RCLCPP_ERROR(this->get_logger(), ex.what());
        } });
    }

  private:
    std::shared_ptr<tf2_ros::TransformListener> listener;
    std::unique_ptr<tf2_ros::Buffer> buffer;
    rclcpp::TimerBase::SharedPtr timer;
  };
}