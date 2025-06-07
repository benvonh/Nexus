call C:\dev\ros2_jazzy\setup.bat
cd C:\Projects\DigitalTwin
colcon build --event-handlers console_cohesion+ desktop_notification- --cmake-args -Wno-dev -DCMAKE_BUILD_TYPE=RelWithDebInfo