cmake_minimum_required(VERSION 3.23)
project(opengl2)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -D FREEGLUT_STATIC")

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(opengl2 main.cpp InputControl.cpp InputControl.h)

target_link_libraries(opengl2 freeglut_static opengl32 winmm gdi32 glu32.lib)