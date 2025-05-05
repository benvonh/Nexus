#include "dt/bridge/node.hpp"
#include "dt/scene/manager.hpp"
#include "pxr/base/gf/matrix4d.h"
#include "pxr/base/gf/quaternion.h"
#include "pxr/base/gf/rotation.h"
#include "pxr/base/gf/vec3d.h"

namespace dt
{
  namespace bridge
  {
    using namespace std::chrono_literals;

    Node::Node() : rclcpp::Node("digital_twin")
    {
      this->buffer = std::make_unique<tf2_ros::Buffer>(this->get_clock());
      this->listener = std::make_shared<tf2_ros::TransformListener>(*this->buffer);
      this->timer = this->create_wall_timer(3ms, [this]()
                                            {
      try
      {
        auto time = scene::Manager::GetTime();
        auto permit = scene::Manager::GetRobotPermit();
        auto &robot = permit.robot;

        for (auto &[name, xform] : robot.xforms)
        {
          auto look = this->buffer->lookupTransform("odom", name.c_str(), tf2::TimePointZero);
          auto &rotation = look.transform.rotation;
          auto &translation = look.transform.translation;
          pxr::GfQuaternion q(rotation.w, pxr::GfVec3d(rotation.x, rotation.y, rotation.z));
          pxr::GfRotation r(q);
          pxr::GfVec3d t(translation.x, translation.y, translation.z);
          pxr::GfMatrix4d m(r, t);
          xform.Set(m, time);
        }
      }
      catch (const tf2::TransformException &e)
      {
        RCLCPP_ERROR_THROTTLE(this->get_logger(), *this->get_clock(), 1000, e.what());
      } });
    }
  }
}