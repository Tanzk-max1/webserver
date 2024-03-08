cmake_minimum_required(VERSION 3.27)
project(webser)

set(CMAKE_CXX_STANDARD 17)

add_executable(webser main.cpp
        EventLoop.cpp
        Channel.h
        EventLoop.h
)
