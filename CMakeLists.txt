cmake_minimum_required(VERSION 3.25)
project(CGOL_CLGL_Interop)

set(CMAKE_CXX_STANDARD 23)

add_executable(CGOL_CLGL_Interop main.cpp CGOL.cpp CGOL.h oclkern.h oglshader.h gl_cl_util.h)

target_link_libraries(CGOL_CLGL_Interop OpenCL OpenGL GLEW glfw)