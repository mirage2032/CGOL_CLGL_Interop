//
// Created by alx on 5/14/23.
//

#ifndef OPENCL0_CGOL_H
#define OPENCL0_CGOL_H
#define CL_HPP_TARGET_OPENCL_VERSION 300
#define CL_HPP_MINIMUM_OPENCL_VERSION 300

#include <CL/opencl.hpp>
#include <CL/cl_gl.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#include <GLFW/glfw3native.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glx.h>

const int WORKGROUP_SIDE = 16;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

struct PixelRGBA {
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
    cl::ImageGL matrixCells;

    GLFWwindow *window;
    GLuint programOGL;
    GLuint vao;
    GLuint texture;

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
