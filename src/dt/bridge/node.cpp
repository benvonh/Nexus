#include "dt/bridge/node.hpp"

namespace dt
{
namespace bridge
{
Node::Node() : rclcpp::Node("digital_twin")
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

} // namespace bridge
} // namespace dt