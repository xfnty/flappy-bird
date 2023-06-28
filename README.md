<p float="left">

[![Windows Build](https://github.com/xfnty/flappy-bird/actions/workflows/build-windows.yml/badge.svg)](https://github.com/xfnty/flappy-bird/actions/workflows/build-windows.yml)
[![Linux Build](https://github.com/xfnty/flappy-bird/actions/workflows/build-linux.yml/badge.svg)](https://github.com/xfnty/flappy-bird/actions/workflows/build-linux.yml)
[![OSX Build](https://github.com/xfnty/flappy-bird/actions/workflows/build-macos.yml/badge.svg)](https://github.com/xfnty/flappy-bird/actions/workflows/build-macos.yml)

</p>

**TODO:**
- [x] Base and background
- [x] Pipes
- [x] Bird
- [x] Score
- [ ] Bird animations
- [ ] Game Over screen
- [ ] Start The Game screen


## Developing

### Installing dependencies
On Linux
```
sudo apt install -y build-essential git cmake libasound2-dev mesa-common-dev libx11-dev libxrandr-dev libxi-dev xorg-dev libgl1-mesa-dev libglu1-mesa-dev
```
On Windows
- [Install Python3](https://www.python.org/downloads/)
- [Install CMake](https://cmake.org/download/)
- [Check out Raylib installation guide](https://github.com/raysan5/raylib/wiki/Working-on-Windows) (everything should work anyway)

### Building
Run `toolchain.py` from the project root:
```
python toolchain.py -cbr
```
Run `./toolchain.py -h` to see all available options.
