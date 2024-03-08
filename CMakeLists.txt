cmake_minimum_required(VERSION 3.27)
project(webser)

set(CMAKE_CXX_STANDARD 17)

add_executable(webser main.cpp
        cmake-build-debug/EventLoop.h
        EventLoop.cpp)
