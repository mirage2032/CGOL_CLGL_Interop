# Conway's Game of Life implementation using OpenGL/OpenCL interoperability

This is a Conway's Game of Life cellular automata computed using OpenCL and rendered using OpenGL.

## Instructions

Just compile and execute it.

The scale of the simulator and of the window can easily be changed in the code.

#### IMPORTANT: Depending on the system the simulation scale might need to be lowered.

## Requirements

* Linux machine (using X11)
* C++ development toolchain
* Necessary libraries: GLFW, GLEW, OpenCL, OpenGL

Tested and working on Arch Linux.

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