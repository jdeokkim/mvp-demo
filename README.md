# MVP Transform Visualizer



> "The engines don’t move the ship at all. 
>
> **The ship stays where it is and the engines move the universe around it.**"
>
> — Futurama

A [raylib](https://github.com/raysan5/raylib) demo to visualize the Model, View, and Projection (MVP) matrices.

This project is a C rewrite of [@diskhkme](https://github.com/diskhkme)'s [MVP Transform Visualizer](https://github.com/diskhkme/mvp_transform_visualize) with additional features, for the "Computer Graphics" (1214-3005) course at [Chungnam National University](https://computer.cnu.ac.kr/).

https://github.com/user-attachments/assets/a0420cad-6b93-4dd6-bae3-fefab8ae3819

## Controls

- `ALT` + `0`: Draw All Spaces
- `ALT` + `1`: Draw Local Space
- `ALT` + `2`: Draw World Space
- `ALT` + `3`: Draw View Space
- `ALT` + `4`: Draw Clip Space
- `ESC`: Lock/Unlock Observer Camera
- `V`: Show/Hide Player Model Vertices

## Prerequisites

- GCC version 11.4.0+
- Git version 2.34.0+
- GNU Make version 4.3+
- raylib 5.0+

## Building

<details>
<summary>Compiling for Windows</summary>

### [w64devkit](https://github.com/skeeto/w64devkit)

Download the latest release of w64devkit from [here](https://github.com/skeeto/w64devkit/releases), extract the `.zip` file to your working directory, and run `w64devkit.exe`.

```console
$ cd .. && wget https://github.com/raysan5/raylib/archive/refs/tags/5.0.zip
$ unzip 5.0.zip && mv raylib-5.0 raylib
$ cd raylib/src && make
```

```console
$ git clone https://github.com/jdeokkim/mvp-demo
$ cd mvp-demo && make -f Makefile.mingw
```

</details>

<!---------------------------------------------------------------------------->

<details>
<summary>Compiling for GNU/Linux</summary>

### Debian / Ubuntu

```console
$ sudo apt update && sudo apt install libasound2-dev libgl1-mesa-dev \
  libglu1-mesa-dev libx11-dev libxrandr-dev libxi-dev libxcursor-dev \
  libxinerama-dev libxkbcommon-dev
$ git clone https://github.com/raysan5/raylib ~/raylib && cd ~/raylib/src
$ make PLATFORM=PLATFORM_DESKTOP GLFW_LINUX_ENABLE_WAYLAND=OFF && make install
```

```console
$ git clone https://github.com/jdeokkim/mvp-demo
$ cd mvp-demo && make
```

</details>

<!---------------------------------------------------------------------------->

<details>
<summary>Compiling for the Web (LLVM-to-WebAssembly)</summary>

Compiling for the Web requires installation of the [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html).

### Debian / Ubuntu

```console
$ git clone git clone https://github.com/emscripten-core/emsdk && cd emsdk
$ ./emsdk install latest
$ ./emsdk activate latest
$ source ./emsdk_env.sh
```

Then, you must recompile raylib for the Web:

```console
$ git clone https://github.com/raysan5/raylib && cd raylib/src
$ make -j`nproc` PLATFORM=PLATFORM_WEB -B
```

Finally, in order to build this project, do:

```
$ git clone https://github.com/jdeokkim/mvp-demo
$ cd mvp-demo && make -f Makefile.emcc
```

</details>

<!---------------------------------------------------------------------------->

<details>
<summary>Cross-compiling from GNU/Linux to Windows (WSL2)</summary>

### Debian / Ubuntu

```console
$ sudo apt install mingw-w64
$ git clone https://github.com/raysan5/raylib && cd raylib/src
$ make CC=x86_64-w64-mingw32-gcc AR=x86_64-w64-mingw32-ar OS=Windows_NT
```

```console
$ git clone https://github.com/jdeokkim/mvp-demo
$ cd mvp-demo && make -f Makefile.mingw
```

</details>

## License

MIT License

```
Copyright (c) 2024 Jaedeok Kim <jdeokkim@protonmail.com>
Copyright (c) 2024 Minhu Lee <fghkk12d@gmail.com>

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
