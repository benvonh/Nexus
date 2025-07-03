cmake_minimum_required(VERSION 3.28.3)
project(digital_twin)

set(TARGET app)
set(CMAKE_CXX_STANDARD 23)
set(SDL_STATIC ON CACHE BOOL "Enable SDL static build" FORCE)
set(SDL_SHARED OFF CACHE BOOL "Disable SDL shared build" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "Disable Assimp install" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "Disable Assimp export functionality" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Disable Assimp tests" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Disable shared libraries" FORCE)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_compile_options(/Zc:preprocessor)

add_compile_definitions(IMGUI_ENABLE_FREETYPE)
add_compile_definitions(HOST_BUILD)

# https://github.com/microsoft/STL/issues/3063
add_compile_definitions(_USE_DETAILED_FUNCTION_NAME_IN_SOURCE_LOCATION=0)

# https://forum.aousd.org/t/issue-building-simple-app-using-usd-c-librairies/1761
add_compile_definitions(NOMINMAX)

# warning C4996: 'std::atomic_compare_exchange_weak_explicit':
# warning STL4029: std::atomic_*() overloads for shared_ptr are deprecated in C++20.
# The shared_ptr specialization of std::atomic provides superior functionality.
add_compile_definitions(_SILENCE_CXX20_OLD_SHARED_PTR_ATOMIC_SUPPORT_DEPRECATION_WARNING)

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

find_package(OpenGL REQUIRED)
find_package(pxr REQUIRED PATHS C:/pixi_ws/usd)
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(urdf REQUIRED)

add_executable(${TARGET}
  ${ImGUI_SOURCE_DIR}/imgui.cpp
  ${ImGUI_SOURCE_DIR}/imgui_demo.cpp
  ${ImGUI_SOURCE_DIR}/imgui_draw.cpp
  ${ImGUI_SOURCE_DIR}/imgui_tables.cpp
  ${ImGUI_SOURCE_DIR}/imgui_widgets.cpp
  ${ImGUI_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
  ${ImGUI_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
  ${ImGUI_SOURCE_DIR}/misc/freetype/imgui_freetype.cpp
  src/dt/app/application.cpp
  src/dt/gui/sub/controller.cpp
  src/dt/gui/sub/parameter.cpp
  src/dt/gui/render.cpp
  src/dt/gui/window.cpp
  src/dt/usd/world.cpp
  src/main.cpp)

target_compile_features(${TARGET} PUBLIC c_std_99 cxx_std_${CMAKE_CXX_STANDARD})

target_include_directories(${TARGET} PRIVATE 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
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
  assimp)

install(TARGETS ${TARGET} DESTINATION lib/${PROJECT_NAME})

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