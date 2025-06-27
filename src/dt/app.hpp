#pragma once
#include "dt/bridge/node.hpp"
#include "dt/view/window.hpp"
#include "dt/view/filedialog.hpp"

#include <memory>
#include <thread>

namespace dt
{
  class App
  {
  public:
    App(int argc, char **argv);
    ~App();

    App(App &&) = delete;
    App(const App &) = delete;
    App &operator=(App &&) = delete;
    App &operator=(const App &) = delete;

    void SpinROS();
    void MainLoop();

  private:
    void _Exec_ROS();
    void _Throw_From_ROS();

    std::mutex _mutex;
    std::jthread _ros_thread;
    std::exception_ptr _exception;
    
    view::Window _window;
    view::FileDialog _file_dialog{*_window};
  };
}