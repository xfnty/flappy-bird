<p float="left">

[![Windows Build](https://github.com/xfnty/flappy-bird/actions/workflows/build-windows.yml/badge.svg)](https://github.com/xfnty/flappy-bird/actions/workflows/build-windows.yml)
[![Linux Build](https://github.com/xfnty/flappy-bird/actions/workflows/build-linux.yml/badge.svg)](https://github.com/xfnty/flappy-bird/actions/workflows/build-linux.yml)
[![OSX Build](https://github.com/xfnty/flappy-bird/actions/workflows/build-macos.yml/badge.svg)](https://github.com/xfnty/flappy-bird/actions/workflows/build-macos.yml)

</p>


<img src="gameplay.gif" width="450" />


**TODO:**
- [x] Base and background
- [x] Pipes
- [x] Bird
- [x] Score
- [x] Bird animations
- [x] Game Over screen
- [x] Start The Game screen
- [ ] Audio
- [ ] Fix segfaults


## Developing

### Installing dependencies
On Linux
```
sudo apt install -y build-essential git cmake libasound2-dev mesa-common-dev libx11-dev libxrandr-dev libxi-dev xorg-dev libgl1-mesa-dev libglu1-mesa-dev
```
On Windows
- [Install CMake](https://cmake.org/download/)
- [Check out Raylib installation guide](https://github.com/raysan5/raylib/wiki/Working-on-Windows) (everything should work anyway)

### Building
```
make configure build run
```
