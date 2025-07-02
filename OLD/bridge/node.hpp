#pragma once
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "tf2_ros/transform_listener.h"
#include "tf2_ros/buffer.h"

namespace dt
{
  namespace bridge
  {
    class Node : public rclcpp::Node
    {
      using Timer = rclcpp::Timercore;
      using TF_Buffer = tf2_ros::Buffer;
      using TF_Listener = tf2_ros::TransformListener;

    public:
      Node();
      ~Node() = default;
      Node(Node &&) = default;
      Node(const Node &) = default;
      Node &operator=(Node &&) = default;
      Node &operator=(const Node &) = default;

    private:
      std::shared_ptr<Timer> timer;
      std::unique_ptr<TF_Buffer> buffer;
      std::shared_ptr<TF_Listener> listener;
    };
  }
}