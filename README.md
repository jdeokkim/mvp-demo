# mvp-demo

> "The engines don’t move the ship at all. 
> **The ship stays where it is and the engines move the universe around it.**"
>
> — Futurama

A [raylib](https://github.com/raysan5/raylib) demo to visualize the Model, View, and Projection (MVP) matrices.

This project is a C rewrite of [@diskhkme](https://github.com/diskhkme)'s [MVP Transform Visualizer](https://github.com/diskhkme/mvp_transform_visualize) with additional features, for the "Computer Graphics" (1214-3005) course at Chungnam National University.

## Screenshots

![Screenshot #1](res/images/SCREENSHOT-01.png)

## Prerequisites

### Debian / Ubuntu

```console
$ sudo apt update && sudo apt install build-essential git
$ sudo apt install libasound2-dev libgl1-mesa-dev libglu1-mesa-dev libx11-dev \
  libxrandr-dev libxi-dev libxcursor-dev libxinerama-dev libxkbcommon-dev
$ git clone https://github.com/team-iwd/mvp-demo ~/mvp-demo
```

### Windows 10+

Download the latest release of w64devkit from [here](https://github.com/skeeto/w64devkit/releases), extract the `.zip` file to your working directory, and run `w64devkit.exe`.

```console
$ mkdir ~/workspace && cd ~/workspace
$ wget https://github.com/team-iwd/mvp-demo/archive/refs/heads/main.zip
$ unzip main.zip -d mvp-demo
```

## Building

```console
$ cd ~/mvp-demo && make
```

## License

MIT License

```
Copyright (c) 2024 Jaedeok Kim <jdeokkim@protonmail.com>

Copyright (c) 2024 Hyungki Kim <diskhkme@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```