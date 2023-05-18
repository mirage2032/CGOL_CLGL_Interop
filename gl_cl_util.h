//
// Created by alx on 5/18/23.
//

#ifndef OPENCL_CGOL_GL_CL_UTIL_H
#define OPENCL_CGOL_GL_CL_UTIL_H

void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message,
                   const void *userParam) {
    // Print or handle the debug message as needed
    std::cerr << "OpenGL Debug Message: " << message << std::endl;
}

GLuint genVAO() {
    GLfloat vertices[] = {
            -1.0f, -1.0f, 0.0f,  // Bottom-left vertex
            1.0f, -1.0f, 0.0f,   // Bottom-right vertex
            1.0f, 1.0f, 0.0f,    // Top-right vertex
            -1.0f, 1.0f, 0.0f    // Top-left vertex
    };

    GLfloat texCoords[] = {
            0.0f, 1.0f,  // Bottom-left texture coordinate
            0.0f, 0.0f,  // Top-left texture coordinate
            1.0f, 0.0f,  // Top-right texture coordinate
            1.0f, 1.0f   // Bottom-right texture coordinate
    };


    GLuint indices[] = {
            0, 1, 2,   // First triangle
            2, 3, 0    // Second triangle
    };
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

#endif //OPENCL_CGOL_GL_CL_UTIL_H
