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
public:
  Node();
  ~Node() = default;
  Node(Node &&) = default;
  Node(const Node &) = default;
  Node &operator=(Node &&) = default;
  Node &operator=(const Node &) = default;

private:
  std::shared_ptr<tf2_ros::TransformListener> listener;
  std::unique_ptr<tf2_ros::Buffer> buffer;
  rclcpp::TimerBase::SharedPtr timer;
};
} // namespace bridge
} // namespace dt