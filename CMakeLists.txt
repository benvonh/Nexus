cmake_minimum_required(VERSION 3.31.3)
project(digital_twin)

set(TARGET app)
set(CMAKE_CXX_STANDARD 23)
set(SDL_STATIC ON CACHE BOOL "Enable static library build" FORCE)
set(SDL_SHARED OFF CACHE BOOL "Disable shared library build" FORCE)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

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
  GIT_TAG v1.91.8-docking)

FetchContent_Declare(STB
  GIT_REPOSITORY https://github.com/nothings/stb)

FetchContent_MakeAvailable(SDL3 ImGUI STB)

set(ImGUI_SOURCE_DIR ${CMAKE_SOURCE_DIR}/build/${PROJECT_NAME}/_deps/imgui-src)
set(STB_SOURCE_DIR ${CMAKE_SOURCE_DIR}/build/${PROJECT_NAME}/_deps/stb-src)

find_package(OpenGL REQUIRED)
find_package(pxr REQUIRED PATHS C:/usd)
find_package(ament_cmake REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(rclcpp REQUIRED)
find_package(urdf REQUIRED)

add_executable(${TARGET}
  ${ImGUI_SOURCE_DIR}/imgui.cpp
  ${ImGUI_SOURCE_DIR}/imgui_demo.cpp
  ${ImGUI_SOURCE_DIR}/imgui_draw.cpp
  ${ImGUI_SOURCE_DIR}/imgui_tables.cpp
  ${ImGUI_SOURCE_DIR}/imgui_widgets.cpp
  ${ImGUI_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
  ${ImGUI_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
  src/window.cpp
  src/main.cpp)

target_compile_features(${TARGET} PUBLIC c_std_99 cxx_std_${CMAKE_CXX_STANDARD})

target_include_directories(${TARGET} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>)

target_include_directories(${TARGET} PRIVATE 
  ${SDL3_SOURCE_DIR}/include
  ${ImGUI_SOURCE_DIR}/backends
  ${ImGUI_SOURCE_DIR}
  ${PXR_INCLUDE_DIRS}
  ${STB_SOURCE_DIR})

target_link_libraries(${TARGET}
  ${PXR_LIBRARIES}
  SDL3::SDL3-static
  OpenGL::GL)

ament_target_dependencies(${TARGET} rclcpp tf2_ros sensor_msgs urdf)

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