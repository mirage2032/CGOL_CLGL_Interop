cmake_minimum_required(VERSION 3.25)
project(OpenCL_CGOL)

set(CMAKE_CXX_STANDARD 23)

add_executable(OpenCL_CGOL main.cpp CGOL.cpp CGOL.h oclkern.h oglshader.h)

target_link_libraries(OpenCL_CGOL OpenCL)
target_link_libraries(OpenCL_CGOL OpenGL)
target_link_libraries(OpenCL_CGOL glfw)
target_link_libraries(OpenCL_CGOL GLEW)
target_link_libraries(OpenCL_CGOL GLX)
target_link_libraries(OpenCL_CGOL X11)

