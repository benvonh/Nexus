# Digital Twin

## Dependencies
- ROS2
- OpenUSD

**USD Install Quick Guide**
1. Clone [OpenUSD](https://github.com/PixarAnimationStudios/OpenUSD) and switch to a release branch (tested on v25.02)
2. Remove the `--` from the CMake command string in `.\build_scripts\build_usd.py`
3. Run `python .\build_scripts\build_usd.py C:\usd` (assume `C:\usd` as output)
4. Download `python313.dll` into `C:\usd\lib`
5. Edit the environment variables so that `PATH` has `C:\usd\bin;C:\usd\lib` and `PYTHONPATH` has `C:\usd\lib\python`

## Usage
Execute `build.bat` then `run.bat`.


## Limitations of ROS2 on Windows 11
- Subscription to the conventional `/robot_description` topic does not yield results