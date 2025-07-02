#include "dt/app.hpp"
#include "rclcpp/rclcpp.hpp"

dt::App::App(int argc, char **argv)
{
  rclcpp::init(argc, argv);
}

dt::App::~App()
{
  rclcpp::shutdown();
}

void dt::App::SpinThread()
{
  _thread = std::jthread(_Exec_ROS, this);
}

void dt::App::MainLoop()
{
  while (_window)
  {
    try
    {
      _window.Update();
      _Throw_From_ROS();
    }
    catch (const dt::custom_exception &e)
    {
      // Only handle DT-native exceptions;
      // standard ones are free to unwind
      // back to the main function.
      _window.ShowException(e);
    }
  }
}

void dt::App::_Exec_ROS()
{
  while (true)
  {
    try
    {
      auto node = std::make_shared<dt::bridge::Node>();
      auto exec = rclcpp::executors::MultiThreadedExecutor();
      exec.add_node(node);
      exec.spin();
      break;
    }
    catch (const std::exception &e)
    {
      while (true)
      {
        std::this_thread::yield();
        std::lock_guard _(_mutex);

        if (!_exception)
        {
          _exception = std::make_exception_ptr(e);
          break;
        }
      }
    }
  }
}

void dt::App::_Throw_From_ROS()
{
  std::lock_guard _(_mutex);

  if (!_exception)
    return;

  std::exception_ptr e;
  std::swap(_exception, e);
  std::rethrow_exception(e);
}