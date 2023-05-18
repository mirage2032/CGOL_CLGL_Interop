# Conway's Game of Life implementation using OpenGL/OpenCL interoperability

This is a Conway's Game of Life cellular automata computed using OpenCL and rendered using OpenGL.

## Instructions

Set the scale of the simulation and of the window in the source code.

<mark>Depending on the system the simulation scale might need to be lowered.</mark>

Make sure that the same GPU is used by both the OpenCL and the OpenGL contexts.

Set the following environment variables
### Nvidia
```bash
export __NV_PRIME_RENDER_OFFLOAD=1
export __GLX_VENDOR_LIBRARY_NAME=nvidia
```
### AMD
```bash
export DRI_PRIME=1
```

<mark>This is really needed as without using the same GPU for the contexts the memory would need to be copied between them. </mark>


GPUs.

Execute it

## Requirements

* Linux machine (using X11)
* C++ development toolchain
* Necessary libraries: GLFW, GLEW, OpenCL, OpenGL

## Results

At the moment using the following specifications:

    OS: Arch Linux x86_64
    Kernel: 6.3.1-arch2-1
    CPU: AMD Ryzen 7 5800H with Radeon Graphics (16) @ 3.200GHz
    GPU: NVIDIA GeForce RTX 3060 Mobile / Max-Q
    Memory: 16GB

Simulation size: 16384 on both the X and Y axis

```cpp
main.cpp

struct {
        int x=1 << 14;
        int y=1 << 14;
    } simulationSize;
```

Window size: 1080x720

```cpp
CGOL.h

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;
```

Fps: 30

```angular2html
Current FPS: 30.4071    
```