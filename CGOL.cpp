//
// Created by alx on 5/14/23.
//
#define CL_HPP_ENABLE_EXCEPTIONS

#include <iostream>
#include <cmath>
#include "CGOL.h"
#include "oclkern.h"

static void buildprogram(cl::Context &ctx, cl::Program *program) {
    try {
        program->build();
    } catch (const cl::Error &e) {
        if (e.err() == CL_BUILD_PROGRAM_FAILURE) {
            // Get the build log and print it out
            std::string log = program->getBuildInfo<CL_PROGRAM_BUILD_LOG>(ctx.getInfo<CL_CONTEXT_DEVICES>()[0]);
            std::cerr << "Build log:" << std::endl << log << std::endl;
        }
        throw e;
    }
}

CGOL::CGOL(int width, int height) : width(width), height(height) {
    //create OpenCL context and queue
    context = cl::Context(CL_DEVICE_TYPE_GPU);
    cl::Device device = context.getInfo<CL_CONTEXT_DEVICES>()[0];
    queue = cl::CommandQueue(context, device);

    //create buffer for the cells
    cl_int error;
    matrixCells = cl::Image2D(context, CL_MEM_READ_WRITE,
                              {CL_RGBA, CL_UNSIGNED_INT8},
                              width, height,
                              0, nullptr,
                              &error);

    std::string OCLDeviceName;
    std::string OCLDeviceVersion;
    device.getInfo(CL_DEVICE_NAME, &OCLDeviceName);
    std::cout << "OpenCL Device: " << OCLDeviceName << std::endl;
    device.getInfo(CL_DEVICE_VERSION, &OCLDeviceVersion);
    std::cout << "OpenCL Version: " << OCLDeviceVersion << std::endl;

    //Build OpenCL Program

    program = cl::Program(context, kernel_randomize);
    buildprogram(context, &program);

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
    }

    // Create a OpenGL window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Use dedicated GPU
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pixel Renderer", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    // Initialize GLEW
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
        glfwTerminate();
    }

    const GLubyte *renderer = glGetString(GL_RENDERER);
    std::cout << "OpenGL Renderer: " << renderer << std::endl;

}

void CGOL::runOCLKernel(cl::Kernel &kernel) {
    try {
        cl::NDRange local(WORKGROUP_SIDE, WORKGROUP_SIDE);
        cl::NDRange global(((width + WORKGROUP_SIDE - 1) / WORKGROUP_SIDE) * WORKGROUP_SIDE,
                           ((height + WORKGROUP_SIDE - 1) / WORKGROUP_SIDE) * WORKGROUP_SIDE);
        queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local);
    }
    catch (cl::Error &error) {
        // Print the error details
        std::cerr << "OpenCL Error: " << error.what() << " (" << error.err() << ")" << std::endl;
    }
}

std::vector<PixelRGBA> CGOL::Read() {
    std::vector<PixelRGBA> imageData(width * height);
    queue.enqueueReadImage(matrixCells, CL_TRUE,
                           {0, 0, 0},
                           {width, height, 1},
                           0, 0,
                           imageData.data());
    return imageData;
}

void CGOL::Randomize() {
    cl::Kernel kernel(program, "randomizeMatrix");
    //add parameters
    kernel.setArg(0, matrixCells);
    kernel.setArg(1, (ulong) time(nullptr));
    //run
    runOCLKernel(kernel);
}

void CGOL::Step() {
    cl::Kernel kernel(program, "stepMatrix");
    //add parameters
    kernel.setArg(0, matrixCells);
    //run
    runOCLKernel(kernel);
}

void CGOL::Render() {

}

void CGOL::Start() {
    while (!glfwWindowShouldClose(window))
    {
        // Process events
        glfwPollEvents();

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        // Render
        Render();

        // Swap buffers
        glfwSwapBuffers(window);
    }
}