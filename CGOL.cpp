//
// Created by alx on 5/14/23.
//
#define CL_HPP_ENABLE_EXCEPTIONS

#include <iostream>
#include "CGOL.h"
#include "oclkern.h"
#include "oglshader.h"
#include "gl_cl_util.h"

void CGOL::initGL() {
    GLint size;
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &size);
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
    }

    // Create a OpenGL window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // Use dedicated GPU
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Proc", NULL, NULL);
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

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(debugCallback, nullptr);

    programOGL = buildOGLProgram(vertexShaderSource, fragmentShaderTextureSource);
    fullscreenMesh = genPlaneMesh();

    // Generate and bind the texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Set texture parameters
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void CGOL::initCL() {

    //Create the OpenCL context/queue/image

    cl_context_properties properties[] = {
            CL_GL_CONTEXT_KHR, (cl_context_properties) glfwGetGLXContext(window),
            CL_GLX_DISPLAY_KHR, (cl_context_properties) glfwGetX11Display(),
            CL_CONTEXT_PLATFORM, (cl_context_properties) (cl::Platform::getDefault()()),
            0
    };

    context = cl::Context(CL_DEVICE_TYPE_GPU, properties);
    cl::Device device = context.getInfo<CL_CONTEXT_DEVICES>()[0];
    queue = cl::CommandQueue(context, device);

    //create buffer for the cells
    cl_int error;
    matrixCells = cl::ImageGL(context, CL_MEM_READ_WRITE, GL_TEXTURE_2D, 0, texture, &error);
    std::string OCLDeviceName;
    std::string OCLDeviceVersion;
    device.getInfo(CL_DEVICE_NAME, &OCLDeviceName);
    std::cout << "OpenCL Device: " << OCLDeviceName << std::endl;
    device.getInfo(CL_DEVICE_VERSION, &OCLDeviceVersion);
    std::cout << "OpenCL Version: " << OCLDeviceVersion << std::endl;

    //Build OpenCL Program

    program = cl::Program(context, CGOLOpenCLKernel);
    buildOCLProgram(context, &program);
}

void CGOL::destroyGL() {
    glDeleteTextures(1, &texture);
    destroyMesh(fullscreenMesh);
    glDeleteProgram(programOGL);
    glfwDestroyWindow(window);
    glfwTerminate();
}

CGOL::CGOL(int width, int height) : width(width), height(height) {
    //Initialize GL first as the existing GL texture will be bound to a CL image
    initGL();
    initCL();
}

void CGOL::runOCLKernel(cl::Kernel &kernel) {
    clEnqueueAcquireGLObjects(queue(), 1, &matrixCells(), 0, nullptr, nullptr);
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
    queue.finish();
    clEnqueueReleaseGLObjects(queue(), 1, &matrixCells(), 0, nullptr, nullptr);
}

std::vector<PixelRGBA> CGOL::Read() {
    std::vector<PixelRGBA> imageData(width * height);

    clEnqueueAcquireGLObjects(queue(), 1, &matrixCells(), 0, nullptr, nullptr);
    queue.enqueueReadImage(matrixCells, CL_TRUE,
                           {0, 0, 0},
                           {width, height, 1},
                           0, 0,
                           imageData.data());
    clEnqueueReleaseGLObjects(queue(), 1, &matrixCells(), 0, nullptr, nullptr);
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

void CGOL::Start() {
    glfwSwapInterval(0);
    glUseProgram(programOGL);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(programOGL, "textureSampler"), 0);
    glBindVertexArray(fullscreenMesh.vao);
    glBindTexture(GL_TEXTURE_2D, texture);

    double previousTime = glfwGetTime();
    int frameCount = 0;

    while (!glfwWindowShouldClose(window)) {
        // Calculate the elapsed time since the last frame
        double currentTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentTime - previousTime);
        previousTime = currentTime;

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        // Issue draw call to render the VAO
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // Swap buffers and poll for events
        glfwSwapBuffers(window);
        glfwPollEvents();

        Step();
//        // Limit FPS
//        // Delay to maintain desired frame rate
//        float targetFrameTime = 1.0f / 1.0f; // 60 FPS
//        float remainingTime = targetFrameTime - deltaTime;
//        if (remainingTime > 0) {
//            // Sleep to maintain frame rate
//            glfwWaitEventsTimeout(targetFrameTime-deltaTime);
//        }
        frameCount++;

        // Calculate and print current frames per second
        float fps = frameCount / deltaTime;
        std::cout << "Current FPS: " << fps << "      \r";
        frameCount = 0;

    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

CGOL::~CGOL() {
    destroyGL();
}
