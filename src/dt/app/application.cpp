#include "application.h"

#include "dt/core/filedialog.h"
#include "dt/core/window.h"
#include "dt/event/client.h"
#include "dt/exception.h"
#include "dt/logging.h"
#include "dt/render/render.h"
#include "dt/ui/log_history.h"
#include "dt/ui/scene_hierarchy.h"
#include "dt/ui/viewports.h"

#include "rclcpp/rclcpp.hpp"

dt::Application::Application(int argc, char **argv)
{
    rclcpp::init(argc, argv);
}

dt::Application::~Application()
{
    rclcpp::shutdown();
}

void dt::Application::SpinThread()
{
    // _Thread = std::jthread(&dt::Application::_Exec_ROS, this);
}

void dt::Application::MainLoop()
{
    Window window;
    Viewports viewports;
    FileDialog fileDialog(*window);

    while (window)
    {
        try
        {
            window.StartFrame();

            draw_log_history();
            draw_scene_hierarchy();

            // if (ImGui::BeginMainMenuBar())
            // {
            //     if (ImGui::BeginMenu("Render Viewort"))
            //     {
            //         ImGui::Text("Viewports: %d/%d", renderCount, sizeof(render));

            //         if (ImGui::Button("Add Render Viewport"))
            //         {
            //             if (renderCount < sizeof(render))
            //             {
            //                 renderCount++;
            //                 render[renderCount - 1].Reset();
            //             }
            //         }
            //         if (ImGui::Button("Remove Render Viewport"))
            //         {
            //             if (renderCount > 1)
            //             {
            //                 renderCount--;
            //             }
            //         }
            //         ImGui::EndMenu();
            //     }
            //     ImGui::EndMainMenuBar();
            // }

            // for (int i = 0; i < renderCount; ++i)
            // {
            //     render[i].Draw();
            // _Render_active = i;
            // _Renders[i].enable_free_camera();
            // SDL_SetWindowRelativeMouseMode(_Window, true);
            // io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
            // }
            viewports.DrawAll();

            if (ImGui::Button("Open File Dialog"))
            {
                FileDialog::Show<FileDialog::Mode::OPEN>(
                    [](std::string filename, int filter) {
                        log::event("Selected file: {} with filter {}", filename, filter);
                    },
                    FileDialog::IMAGE_FILTER);
            }

            window.FinishFrame();
            window.HandleEvents();

            Client::Dispatch();

            _Throw_from_ROS();
        }
        catch (const __exception__ &e)
        {
            window.ShowException(e);
        }
    }
}

void dt::Application::_Exec_ROS()
{
    // while (true)
    // {
    //     try
    //     {
    //         // auto node = std::make_shared<dt::bridge::Node>();
    //         // auto exec = rclcpp::executors::MultiThreadedExecutor();
    //         // exec.add_node(node);
    //         // exec.spin();
    //         break;
    //     }
    //     catch (const std::exception &e)
    //     {
    //         while (true)
    //         {
    //             std::this_thread::yield();
    //             std::lock_guard guard(_Mutex);

    //             if (_Exception != nullptr)
    //             {
    //                 _Exception = std::make_exception_ptr(e);
    //                 break;
    //             }
    //         }
    //     }
    // }
}

void dt::Application::_Throw_from_ROS()
{
    std::lock_guard guard(_Mutex);

    if (_Exception == nullptr) [[likely]]
        return;

    std::exception_ptr e;
    std::swap(_Exception, e);
    std::rethrow_exception(e);
}