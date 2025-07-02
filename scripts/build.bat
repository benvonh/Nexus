call C:\pixi_ws\ros2-windows\setup.bat
@REM md C:\pixi_ws\DigitalTwin\build
cd C:\pixi_ws\DigitalTwin\build
@REM pixi run --manifest-path C:\pixi_ws cmake ..
pixi run --manifest-path C:\pixi_ws cmake --build . --config RelWithDebInfo -- /m