# Nexus

A simple Digital Twin application to interact with ROS and the Universal Scene Description.

<!-- https://github.com/user-attachments/assets/d0448d48-7468-44f2-b7b0-418af48c90fa -->
![test](./docs/media/test.gif)

**NOTE**: As this is under development, only Windows is currently supported. However, Linux will receive similar support in the near future.

## Installation

### Dependency

:warning: This may take up to an hour and ~50GB of storage!

1. Install [Pixi](https://github.com/prefix-dev/pixi/releases/latest/download/pixi-x86_64-pc-windows-msvc.msi) - default

2. Install [Visual Studio](https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=Community&channel=Release&version=VS2022&source=VSLandingPage&cid=2030&passive=false) - check "Desktop development with C++"

3. Install the ROS2 (Kilted Kaiju) environment

```ps
md C:\pixi_ws
cd C:\pixi_ws
irm https://raw.githubusercontent.com/ros2/ros2/refs/heads/kilted/pixi.toml -OutFile pixi.toml
pixi install
```

4. Download the ROS2 [zip](https://github.com/ros2/ros2/releases/download/release-kilted-20250523/ros2-kilted-20250523-windows-release-amd64.zip) and extract under `C:\pixi_ws`

5. Install OpenUSD from **Developer Command Prompt for VS 2022 Preview**

    - For a less cluttered namespace, edit `pxr/pxr.h.in` and set `PXR_INTERNAL_NS` the same as `PXR_NS`.

```ps
cd C:\pixi_ws
pixi shell
pip install pyopengl pyside6
git clone https://github.com/PixarAnimationStudios/OpenUSD.git
cd OpenUSD
git checkout v25.02
python build_scripts\build_usd.py --build-variant release ..\usd
```

6. Edit the environment variables so that `PATH` has `C:\pixi_ws\usd\bin;C:\pixi_ws\usd\lib` and `PYTHONPATH` has `C:\pixi_ws\usd\lib\python`

7. Install final dependencies via `vcpkg` from the same shell

```ps
cd C:\pixi_ws
git clone https://github.com/microsoft/vcpkg.git
call .\vcpkg\bootstrap-vcpkg.bat
vcpkg install freetype
vcpkg integrate install
```

### Verify Setup

- ROS2

```ps
call C:\pixi_ws\ros2-windows\setup.bat
ros2 topic list
```

- OpenUSD

```ps
cd C:\pixi_ws
pixi shell
usdview usd\share\usd\tutorials\authoringProperties\HelloWorld.usda
```

### Digital Twin

Clone this repository under `C:\pixi_ws` then follow the usual steps for a CMake project.
Ensure `--config` is consistent with what OpenUSD was built with.

```ps
cmake -S . -B build
cmake --build build --target app --config Release -- /m
.\Release\app.exe
```

Scripts are also provided which can be called/executed from anywhere.

## Notes

1. To set up a proper development environment, enter the following in Command Prompt.

```ps
cd C:\pixi_ws
pixi shell
call ros2-windows\setup.bat

devenv     # Visual Studio
code Nexus # Visual Studio Code
```

2. Due to immense difficulty in configuring OpenUSD through CMake (Visual Studio multi-config), a single config must be specified. To change later, delete `C:\pixi_ws\usd` and build from scratch again. I'm not sure of the proper way to do this at the moment.

3. Linking against OpenUSD as a static library on Windows does NOT currently work. The default is shared. See https://github.com/PixarAnimationStudios/OpenUSD/issues/3079.

4. ROS2 does not provide packaged binaries for Debug on Windows; but building ROS2 yourself does not work for Debug anyway plus other CMake issues for a full build. It is recommended to build OpenUSD in Release mode with Debug Info instead.

## Roadmap

- Showcase with Franka Panda
    - Even try running simple RL on it
- Implement live editing of properties and prims
- Switch to more advanced rendering like https://github.com/GPUOpen-LibrariesAndSDKs/RadeonProRenderUSD
    - Also looked at Autodesks's Aurora
    - Seems like nothing really works out the box so put on hold
- Investigate how to do VR imaging
    - Need to look at ALVR, OpenXR

## Third Party Software

- OpenUSD
- ROS2
- SDL3
- Dear ImGui
- Assimp
- STB
- Termcolor
