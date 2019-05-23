cmake_minimum_required(VERSION 3.8)
project(emulator)

set(CMAKE_CXX_STANDARD 11)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unknown-pragmas -Wno-unused-function -Wno-unused-parameter")

set(SOURCE_FILES mBotEmul.cpp winMain.cpp mbotProgram.cpp)
add_executable(emulator ${SOURCE_FILES})