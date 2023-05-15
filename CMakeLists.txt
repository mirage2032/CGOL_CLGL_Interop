cmake_minimum_required(VERSION 3.25)
project(opencl0)

set(CMAKE_CXX_STANDARD 23)

find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
add_executable(opencl0 main.cpp CGOL.cpp CGOL.h oclkern.h)

target_link_libraries(opencl0 OpenCL)
target_link_libraries(opencl0 glfw)
target_link_libraries(opencl0 GLEW)
target_link_libraries(opencl0 OpenGL)

