call C:\pixi_ws\ros2-windows\setup.bat
cd C:\pixi_ws\DigitalTwin\build
@REM pixi run --manifest-path C:\pixi_ws cmake --build . --target app -- /m
pixi run --manifest-path C:\pixi_ws cmake --build . --target app --config Release -- /m