cmake_minimum_required(VERSION 3.28.3)
project(digital_twin)

################################################################################
# Set Initial Variables
################################################################################
set(TARGET app)
set(CMAKE_CXX_STANDARD 23)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Disable Assimp tests" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "Disable Assimp install" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "Disable Assimp export functionality" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Disable shared libraries" FORCE)
set(SDL_DISABLE_INSTALL ON CACHE BOOL "Disable SDL install" FORCE)
set(SDL_DISABLE_INSTALL_DOCS ON CACHE BOOL "Disable SDL install docs" FORCE)
set(SDL_INSTALL_TESTS OFF CACHE BOOL "Disable SDL install tests" FORCE)
set(SDL_SHARED OFF CACHE BOOL "Disable SDL shared build" FORCE)
set(SDL_STATIC ON CACHE BOOL "Enable SDL static build" FORCE)
set(SDL_TESTS OFF CACHE BOOL "Disable SDL tests" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "Disable SDL test library" FORCE)

################################################################################
# Set Platform Specifics
################################################################################
if(WIN32)

  add_compile_options(/Zc:preprocessor)

elseif(UNIX)

  if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
  endif()

endif()

################################################################################
# Download Vendors
################################################################################
include(FetchContent)

FetchContent_Declare(SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL
  GIT_TAG release-3.2.4)

FetchContent_Declare(ImGUI
  GIT_REPOSITORY https://github.com/ocornut/imgui
  GIT_TAG v1.92.0-docking)

FetchContent_Declare(Assimp
  GIT_REPOSITORY https://github.com/assimp/assimp
  GIT_TAG v6.0.1)

FetchContent_Declare(Termcolor
  GIT_REPOSITORY https://github.com/ikalnytskyi/termcolor)

FetchContent_Declare(STB
  GIT_REPOSITORY https://github.com/nothings/stb)

FetchContent_MakeAvailable(SDL3 ImGUI Assimp Termcolor STB)

set(ImGUI_SOURCE_DIR ${CMAKE_SOURCE_DIR}/build/_deps/imgui-src)
set(Termcolor_SOURCE_DIR ${CMAKE_SOURCE_DIR}/build/_deps/termcolor-src)
set(STB_SOURCE_DIR ${CMAKE_SOURCE_DIR}/build/_deps/stb-src)

################################################################################
# Find Packages
################################################################################
find_package(OpenGL REQUIRED)
find_package(pxr REQUIRED PATHS C:/pixi_ws/usd)
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(urdf REQUIRED)

################################################################################
# Add ImGUI Target
################################################################################
add_library(ImGUI STATIC 
  ${ImGUI_SOURCE_DIR}/imgui.cpp
  ${ImGUI_SOURCE_DIR}/imgui_demo.cpp
  ${ImGUI_SOURCE_DIR}/imgui_draw.cpp
  ${ImGUI_SOURCE_DIR}/imgui_tables.cpp
  ${ImGUI_SOURCE_DIR}/imgui_widgets.cpp
  ${ImGUI_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
  ${ImGUI_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
  ${ImGUI_SOURCE_DIR}/misc/freetype/imgui_freetype.cpp)

target_include_directories(ImGUI PUBLIC
  ${SDL3_SOURCE_DIR}/include
  ${ImGUI_SOURCE_DIR}/backends
  ${ImGUI_SOURCE_DIR})

target_compile_definitions(ImGUI PRIVATE IMGUI_ENABLE_FREETYPE)

################################################################################
# Add Main Target
################################################################################
add_executable(${TARGET}
  src/dt/app/application.cpp
  src/dt/app/application.h
  src/dt/gui/sub/controller.cpp
  src/dt/gui/sub/controller.h
  src/dt/gui/sub/parameter.cpp
  src/dt/gui/sub/parameter.h
  src/dt/gui/browser.cpp
  src/dt/gui/browser.h
  src/dt/gui/file_dialog.h
  src/dt/gui/logs.cpp
  src/dt/gui/logs.h
  src/dt/gui/render.cpp
  src/dt/gui/render.h
  src/dt/gui/window.cpp
  src/dt/gui/window.h
  src/dt/usd/permit.h
  src/dt/usd/robot.cpp
  src/dt/usd/robot.h
  src/dt/usd/world.cpp
  src/dt/usd/world.h
  src/dt/exception.h
  src/dt/logging.h
  src/dt/utility.h
  src/main.cpp)

target_include_directories(${TARGET} PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${ImGUI_SOURCE_DIR}
  ${ImGUI_SOURCE_DIR}/backends
  ${PXR_INCLUDE_DIRS}
  ${SDL3_SOURCE_DIR}/include
  ${STB_SOURCE_DIR}
  ${Termcolor_SOURCE_DIR}/include/termcolor)

target_link_libraries(${TARGET}
  ${sensor_msgs_TARGETS}
  ${tf2_ros_TARGETS}
  ${urdf_TARGETS}
  ${PXR_LIBRARIES}
  SDL3::SDL3-static
  OpenGL::GL
  assimp
  ImGUI)

# TODO: Not tested on Linux - add here for common definitions
# target_compile_definitions(${TARGET} PRIVATE)

if(WIN32)
  target_compile_definitions(${TARGET} PRIVATE
    # https://forum.aousd.org/t/issue-building-simple-app-using-usd-c-librairies/1761
    NOMINMAX

    # error C1017: invalid integer constant expression
    TBB_USE_DEBUG

    # https://github.com/RenderKit/embree/issues/148
    __TBB_NO_IMPLICIT_LINKAGE
    __TBBMALLOC_NO_IMPLICIT_LINKAGE

    # TRACKEDEXEC : TBB warning : tbb/task.h is deprecated.
    # For details, please see Deprecated Features appendix in the TBB reference manual.
    TBB_SUPPRESS_DEPRECATED_MESSAGES

    # https://github.com/microsoft/STL/issues/3063
    _USE_DETAILED_FUNCTION_NAME_IN_SOURCE_LOCATION=0

    # warning C4996: 'std::atomic_compare_exchange_weak_explicit':
    # warning STL4029: std::atomic_*() overloads for shared_ptr are deprecated in C++20.
    # The shared_ptr specialization of std::atomic provides superior functionality.
    _SILENCE_CXX20_OLD_SHARED_PTR_ATOMIC_SUPPORT_DEPRECATION_WARNING)
endif()

target_compile_features(${TARGET} PUBLIC c_std_99 cxx_std_${CMAKE_CXX_STANDARD})

################################################################################
# Do ROS2 Stuff
################################################################################
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()