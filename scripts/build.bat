cd C:\pixi_ws
pixi shell
call .\ros2-windows\setup.bat
cd .\DigitalTwin
colcon build --event-handlers console_cohesion+ --cmake-args -Wno-dev -DCMAKE_BUILD_TYPE=RelWithDebInfo
pause