//
// Created by alx on 5/14/23.
//

#ifndef OPENCL0_CGOL_H
#define OPENCL0_CGOL_H

#define CL_HPP_TARGET_OPENCL_VERSION 300

#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX

#include <CL/opencl.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

struct MeshData {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
};

const int WORKGROUP_SIDE = 16;

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;

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
    MeshData fullscreenMesh;
    GLuint texture;

    void runOCLKernel(cl::Kernel &kernel);

    void initGL();

    void initCL();

    void destroyGL();

public:
    CGOL(int width, int height);

    void Randomize();

    std::vector<PixelRGBA> Read();

    void Start();

    void Step();
    ~CGOL();
};


#endif //OPENCL0_CGOL_H
