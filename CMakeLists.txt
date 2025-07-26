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

# set(Torch_DIR C:/Users/Ben/.pixi/envs/libtorch/Library/share/cmake/Torch)
# set(CMAKE_CUDA_ARCHITECTURES 86) # RTX 3060 Ti
# set(USE_SYSTEM_NVTX ON)

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
  GIT_TAG release-3.2.16)

FetchContent_Declare(ImGUI
  GIT_REPOSITORY https://github.com/ocornut/imgui
  GIT_TAG v1.92.0-docking)

FetchContent_Declare(Assimp
  GIT_REPOSITORY https://github.com/assimp/assimp
  GIT_TAG v6.0.1)

FetchContent_Declare(STB
  GIT_REPOSITORY https://github.com/nothings/stb)

FetchContent_Declare(Termcolor
  GIT_REPOSITORY https://github.com/ikalnytskyi/termcolor)

FetchContent_MakeAvailable(SDL3 ImGUI Assimp STB Termcolor)

set(ImGUI_SOURCE_DIR ${CMAKE_SOURCE_DIR}/build/_deps/imgui-src)
set(STB_SOURCE_DIR ${CMAKE_SOURCE_DIR}/build/_deps/stb-src)
set(Termcolor_SOURCE_DIR ${CMAKE_SOURCE_DIR}/build/_deps/termcolor-src)

################################################################################
# Find Packages
################################################################################
find_package(OpenGL REQUIRED)

# find_package(CUDAToolkit REQUIRED)
# find_package(Torch REQUIRED)

# find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(urdf REQUIRED)

if(WIN32)

  find_package(pxr REQUIRED PATHS C:/pixi_ws/usd)
  # TODO: Building USD from source is so complicated
  #       - probably tackle this again in the future
  # set(TBB_ROOT_DIR C:/pixi_ws/usd/src/tbb)
  # set(OPENSUBDIV_ROOT_DIR C:/pixi_ws/usd/src/OpenSubdiv-3_6_0)
  # add_subdirectory(C:/pixi_ws/OpenUSD C:/pixi_ws/usd-temp)
  # set(PXR_LIBRARIES boost python arch tf gf pegtl js trace work plug vt ts ar kind sdf ndr sdr pcp usd usdGeom usdVol usdMedia usdShade usdLux usdProc usdRender usdHydra usdRi usdSemantics usdSkel usdUI usdUtils usdPhysics usdMtlx usdValidation usdGeomValidators usdShadeValidators usdSkelValidators usdUtilsValidators garch hf hio cameraUtil pxOsd geomUtil glf hgi hgiGL hgiInterop hd hdar hdGp hdsi hdMtlx hdSt hdx usdImaging usdImagingGL usdProcImaging usdRiPxrImaging usdSkelImaging usdVolImaging usdAppUtils usdviewq usdBakeMtlx)
  # set(PXR_INCLUDE_DIRS C:/pixi_ws/usd/include)

elseif(UNIX)

  find_package(pxr REQUIRED)

endif()

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
set(SOURCE
  src/main.cpp
  src/nexus/app/application.cpp
  src/nexus/app/application.h
  src/nexus/core/world.cpp
  src/nexus/core/world.h
  src/nexus/event/client.h
  src/nexus/event/event.h
  src/nexus/event/keyboard_event.h
  src/nexus/event/mouse_event.h
  src/nexus/event/scene_reset_event.h
  src/nexus/event/viewport_capture_event.h
  src/nexus/exception.h
  src/nexus/logging.h
  src/nexus/render/controller.cpp
  src/nexus/render/controller.h
  src/nexus/render/parameter.h
  src/nexus/render/render.cpp
  src/nexus/render/render.h
  src/nexus/types.h
  src/nexus/utility.h
  src/nexus/view/filedialog.h
  src/nexus/view/panel/log_history.h
  src/nexus/view/panel/menu_bar.h
  src/nexus/view/panel/property.cpp
  src/nexus/view/panel/property.h
  src/nexus/view/panel/scene_hierarchy.cpp
  src/nexus/view/panel/scene_hierarchy.h
  src/nexus/view/panel/viewports.cpp
  src/nexus/view/panel/viewports.h
  src/nexus/view/window.cpp
  src/nexus/view/window.h
  src/nexus/view/window_theme.cpp
  src/nexus/view/window_theme.h)

add_executable(${TARGET} ${SOURCE})

target_include_directories(${TARGET} PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${ImGUI_SOURCE_DIR}
  ${ImGUI_SOURCE_DIR}/backends
  ${PXR_INCLUDE_DIRS}
  ${SDL3_SOURCE_DIR}/include
  ${STB_SOURCE_DIR}
  ${Termcolor_SOURCE_DIR}/include/termcolor)

target_link_libraries(${TARGET}
  ${PXR_LIBRARIES}
  ${sensor_msgs_TARGETS}
  ${tf2_ros_TARGETS}
  ${urdf_TARGETS}
  SDL3::SDL3-static
  OpenGL::GL
  assimp
  ImGUI)

target_compile_definitions(${TARGET} PRIVATE HOST_BUILD)

if(WIN32)
  target_compile_definitions(${TARGET} PRIVATE
    # error C1017: invalid integer constant expression
    $<$<CONFIG:Debug>:TBB_USE_DEBUG>

    # https://github.com/RenderKit/embree/issues/148
    $<$<CONFIG:Debug>:__TBB_NO_IMPLICIT_LINKAGE>
    $<$<CONFIG:Debug>:__TBBMALLOC_NO_IMPLICIT_LINKAGE>

    # https://forum.aousd.org/t/issue-building-simple-app-using-usd-c-librairies/1761
    NOMINMAX

    # TRACKEDEXEC : TBB warning : tbb/task.h is deprecated
    # For details, please see Deprecated Features appendix in the TBB reference manual.
    TBB_SUPPRESS_DEPRECATED_MESSAGES

    # https://github.com/microsoft/STL/issues/3063
    _USE_DETAILED_FUNCTION_NAME_IN_SOURCE_LOCATION=0

    # warning C4996: 'std::atomic_compare_exchange_weak_explicit':
    # warning STL4029: std::atomic_*() overloads for shared_ptr are deprecated in C++20.
    # The shared_ptr specialization of std::atomic provides superior functionality.
    _SILENCE_CXX20_OLD_SHARED_PTR_ATOMIC_SUPPORT_DEPRECATION_WARNING)

  # For Visual Studio to organize files with the same structure as the source tree
  source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${SOURCE})

  # set_target_properties(${TARGET} PROPERTIES WIN32_EXECUTABLE FALSE)
endif()

target_compile_features(${TARGET} PUBLIC c_std_99 cxx_std_${CMAKE_CXX_STANDARD})

# TODO: Debating whether to use precompiled headers
# target_precompile_headers(${TARGET} PRIVATE nexus/pch.h)

################################################################################
# Do ROS2 Stuff
################################################################################
# if(BUILD_TESTING)
#   find_package(ament_lint_auto REQUIRED)
#   # the following line skips the linter which checks for copyrights
#   # comment the line when a copyright and license is added to all source files
#   set(ament_cmake_copyright_FOUND TRUE)
#   # the following line skips cpplint (only works in a git repo)
#   # comment the line when this package is in a git repo and when
#   # a copyright and license is added to all source files
#   set(ament_cmake_cpplint_FOUND TRUE)
#   ament_lint_auto_find_test_dependencies()
# endif()

# ament_package()