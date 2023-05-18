//
// Created by alx on 5/14/23.
//
#define CL_HPP_ENABLE_EXCEPTIONS

#include <iostream>
#include <cmath>
#include "CGOL.h"
#include "oclkern.h"
#include "oglshader.h"


void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message,
                   const void *userParam) {
    // Print or handle the debug message as needed
    std::cerr << "OpenGL Debug Message: " << message << std::endl;
}

GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f,  // Bottom-left vertex
        1.0f, -1.0f, 0.0f,   // Bottom-right vertex
        1.0f, 1.0f, 0.0f,    // Top-right vertex
        -1.0f, 1.0f, 0.0f    // Top-left vertex
};

GLfloat texCoords[] = {
        0.0f, 0.0f,  // Bottom-left texture coordinate
        1.0f, 0.0f,  // Bottom-right texture coordinate
        1.0f, 1.0f,  // Top-right texture coordinate
        0.0f, 1.0f   // Top-left texture coordinate
};

GLuint indices[] = {
        0, 1, 2,   // First triangle
        2, 3, 0    // Second triangle
};

void buildOCLProgram(cl::Context &ctx, cl::Program *program) {
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

GLuint buildOGLShader(GLenum shaderType, const char *sourceCode) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &sourceCode, nullptr);
    glCompileShader(shader);

    // Check for compilation errors
    GLint compileStatus;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE) {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar *infoLog = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(shader, infoLogLength, nullptr, infoLog);
        printf("Shader compilation failed:\n%s\n", infoLog);
        delete[] infoLog;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint buildOGLProgram(const char *vertexShaderSource, const char *fragmentShaderSource) {
    // Compile vertex shader
    GLuint vertexShader = buildOGLShader(GL_VERTEX_SHADER, vertexShaderSource);
    if (vertexShader == 0) {
        return 0;
    }

    // Compile fragment shader
    GLuint fragmentShader = buildOGLShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    if (fragmentShader == 0) {
        return 0;
    }

    // Create shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    // Link the shader program
    glLinkProgram(shaderProgram);

    // Check for linking errors
    GLint linkStatus;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE) {
        GLint infoLogLength;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar *infoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(shaderProgram, infoLogLength, nullptr, infoLog);
        printf("Shader program linking failed:\n%s\n", infoLog);
        delete[] infoLog;
        glDeleteProgram(shaderProgram);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    // Clean up individual shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

GLuint genVAO() {
    GLuint vbo, vao, ebo;
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(texCoords), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(texCoords), texCoords);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set up vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), static_cast<GLvoid *>(nullptr));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(sizeof(vertices)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    return vao;
}

CGOL::CGOL(int width, int height) : width(width), height(height) {

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
    }

    // Create a OpenGL window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
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
    vao = genVAO();

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    GLubyte pixelData[100 * 100 * 4]; // White pixel data (100x100 pixels)

// Fill the pixel data with white color
    for (int i = 0; i < 100 * 100 * 4; i += 4) {
        pixelData[i] = 255;     // Red component
        pixelData[i + 1] = 255; // Green component
        pixelData[i + 2] = 255; // Blue component
        pixelData[i + 3] = 255; // Alpha component
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
    glBindTexture(GL_TEXTURE_2D, 0);


/////////////////////////////
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
//    matrixCells = cl::Image2D(context, CL_MEM_READ_WRITE,
//                              {CL_RGBA, CL_UNSIGNED_INT8},
//                              width, height,
//                              0, nullptr,
//                              &error);

    std::string OCLDeviceName;
    std::string OCLDeviceVersion;
    device.getInfo(CL_DEVICE_NAME, &OCLDeviceName);
    std::cout << "OpenCL Device: " << OCLDeviceName << std::endl;
    device.getInfo(CL_DEVICE_VERSION, &OCLDeviceVersion);
    std::cout << "OpenCL Version: " << OCLDeviceVersion << std::endl;

    //Build OpenCL Program

    program = cl::Program(context, kernel_randomize);
    buildOCLProgram(context, &program);


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
    clEnqueueReleaseGLObjects(queue(), 1, &matrixCells(), 0, nullptr, nullptr);
}

std::vector<PixelRGBA> CGOL::Read() {
    clEnqueueAcquireGLObjects(queue(), 1, &matrixCells(), 0, nullptr, nullptr);
    std::vector<PixelRGBA> imageData(width * height);
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
    queue.finish();

}

void CGOL::Step() {
    cl::Kernel kernel(program, "stepMatrix");
    //add parameters
    kernel.setArg(0, matrixCells);
    //run
    runOCLKernel(kernel);
    queue.finish();
}

void CGOL::Render() {
}

void CGOL::Start() {
    glUseProgram(programOGL);
    glUniform1i(glGetUniformLocation(programOGL, "useTexture"),GL_TRUE);
    glUniform1i(glGetUniformLocation(programOGL, "textureSampler"), texture);
    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0);

    while (!glfwWindowShouldClose(window)) {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, texture);
        clEnqueueAcquireGLObjects(queue(), 1, &matrixCells(), 0, nullptr, nullptr);

        // Issue draw call to render the VAO
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // Swap buffers and poll for events
        glfwSwapBuffers(window);
        glfwPollEvents();

        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glBindVertexArray(0);
}