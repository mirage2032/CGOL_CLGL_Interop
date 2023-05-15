//
// Created by alx on 5/14/23.
//

#ifndef OPENCL0_CGOL_H
#define OPENCL0_CGOL_H

#include <CL/opencl.hpp>
#include <GL/glxew.h>
#include <GLFW/glfw3.h>

const int WORKGROUP_SIDE = 16;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int PIXEL_SIZE = 10;
typedef struct PixelRGBA {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

class CGOL {
private:
    size_t width, height;
    cl::Context context;
    cl::Program program;
    cl::CommandQueue queue;
    cl::Image2D matrixCells;
    GLFWwindow *window;

    void runOCLKernel(cl::Kernel &kernel);
public:
    CGOL(int width, int height);

    void Randomize();

    std::vector<PixelRGBA> Read();

    void Start();
    void Render();

    void Step();
};


#endif //OPENCL0_CGOL_H
