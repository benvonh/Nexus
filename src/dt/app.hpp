#pragma once
#include "dt/bridge/node.hpp"
#include "dt/view/window.hpp"
#include <memory>
#include <thread>

namespace dt
{
  class App
  {
  public:
    App() = default;
    ~App() = default;
    App(App &&) = delete;
    App(const App &) = delete;
    App &operator=(App &&) = delete;
    App &operator=(const App &) = delete;

    void StartROS();
    void MainLoop();

  private:
    void _Exec_ROS();
    void _Throw_From_ROS();

    std::mutex _mutex;
    std::jthread _ros_thread;
    std::exception_ptr _exception;
    view::Window _window;
  };
}