call C:\pixi_ws\ros2-windows\setup.bat
cd C:\pixi_ws\DigitalTwin\build
pixi run --manifest-path C:\pixi_ws cmake --build . --target app -- /m
@REM pixi run --manifest-path C:\pixi_ws cmake --build . --target app --config RelWithDebInfo -- /m