cmake_minimum_required(VERSION 3.8)
project(emulator)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp mainDev.cpp mbotProgram.cpp)
add_executable(emulator ${SOURCE_FILES})