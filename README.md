# Digital Twin

## Installation

### Dependencies

:warning: This may take up to 30 minutes.

1. Install [Pixi](https://github.com/prefix-dev/pixi/releases/latest/download/pixi-x86_64-pc-windows-msvc.msi) - default 
2. Install [Visual Studio](https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=Community&channel=Release&version=VS2022&source=VSLandingPage&cid=2030&passive=false) - check "Desktop development with C++"

3. Install ROS2 (Kilted Kaiju) in Powershell
```ps
md C:\pixi_ws
cd C:\pixi_ws
irm https://raw.githubusercontent.com/ros2/ros2/refs/heads/kilted/pixi.toml -OutFile pixi.toml
pixi install
```

Download the ROS2 [zip](https://github.com/ros2/ros2/releases/download/release-kilted-20250523/ros2-kilted-20250523-windows-release-amd64.zip) and extract to `C:\pixi_ws`

4. Install OpenUSD in Developer Command Prompt for VS 2022 Preview
```ps
cd C:\pixi_ws
pixi shell
pip install pyopengl pyside6
git clone https://github.com/PixarAnimationStudios/OpenUSD.git
cd OpenUSD
git checkout v25.02
python build_scripts\build_usd.py ..\usd
```

Edit the environment variables so that `PATH` has `C:\pixi_ws\usd\bin;C:\pixi_ws\usd\lib` and `PYTHONPATH` has `C:\pixi_ws\usd\lib\python`

### Verify Setup

- ROS2 in Command Prompt
```ps
call C:\pixi_ws\ros2-windows\setup.bat
ros2 topic list
```

- OpenUSD in either
```ps
cd C:\pixi_ws
pixi shell
usdview usd\share\usd\tutorials\authoringProperties\HelloWorld.usda
```

### Digital Twin

Clone this repository and execute the scripts.