# Digital Twin

## Installation

### Dependency

:warning: This may take upto 30 minutes.

1. Install [Pixi](https://github.com/prefix-dev/pixi/releases/latest/download/pixi-x86_64-pc-windows-msvc.msi) - default

2. Install [Visual Studio](https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=Community&channel=Release&version=VS2022&source=VSLandingPage&cid=2030&passive=false) - check "Desktop development with C++"

3. Install the ROS2 (Kilted Kaiju) environment from Powershell

```ps
md C:\pixi_ws
cd C:\pixi_ws
irm https://raw.githubusercontent.com/ros2/ros2/refs/heads/kilted/pixi.toml -OutFile pixi.toml
pixi install
```

4. Download the ROS2 [zip](https://github.com/ros2/ros2/releases/download/release-kilted-20250523/ros2-kilted-20250523-windows-release-amd64.zip) and extract into `C:\pixi_ws`

5. Install OpenUSD from Developer Command Prompt for VS 2022 Preview

```ps
cd C:\pixi_ws
pixi shell
pip install pyopengl pyside6
git clone https://github.com/PixarAnimationStudios/OpenUSD.git
cd OpenUSD
git checkout v25.02
python build_scripts\build_usd.py ..\usd
```

6. Edit the environment variables so that `PATH` has `C:\pixi_ws\usd\bin;C:\pixi_ws\usd\lib` and `PYTHONPATH` has `C:\pixi_ws\usd\lib\python`

7. Install FreeType via `vcpkg` from the same shell

```ps
cd C:\pixi_ws
git clone https://github.com/microsoft/vcpkg.git
call .\vcpkg\bootstrap-vcpkg.bat
vcpkg install freetype
vcpkg integrate install
```

### Verify Setup

- ROS2 in Command Prompt

```ps
call C:\pixi_ws\ros2-windows\setup.bat
ros2 topic list
```

- OpenUSD in any terminal

```ps
cd C:\pixi_ws
pixi shell
usdview usd\share\usd\tutorials\authoringProperties\HelloWorld.usda
```

### Digital Twin

Clone this repository under `C:\pixi_ws` then follow the usual steps for CMake projects.

```ps
md C:\pixi_ws\DigitalTwin\build
cd C:\pixi_ws\DigitalTwin\build
cmake ..
cmake --build . --target app --config Release -- /m
.\Release\app.exe
```

If you are too lazy, simply execute the scripts in order of configure -> build -> run.

If your IDE of choice is Visual Studio, skip the build step - just make note of the below print.

## Notes

1. To set up a proper development environment, enter the following in Command Prompt.

```ps
cd C:\pixi_ws
pixi shell
call ros2-windows\setup.bat

devenv           # Visual Studio
code DigitalTwin # Visual Studio Code
```

2. Debug builds do NOT currently work. Build with RelWithDebInfo instead.

## Roadmap

1. 3D Reconstuction and Map Survey
2. VR Compatiblity
3. Medical Research
