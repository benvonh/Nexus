call C:\pixi_ws\ros2-windows\setup.bat
md C:\pixi_ws\Nexus\build
cd C:\pixi_ws\Nexus\build
pixi run --manifest-path ..\.. cmake ..