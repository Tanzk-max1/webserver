cmake_minimum_required(VERSION 3.27)
project(webser)

set(CMAKE_CXX_STANDARD 17)

add_executable(webser main.cpp
        EventLoop.cpp
        Channel.h
        EventLoop.h
        Epoll.h
        Epoll.cpp
        base/LogStream.cpp
        base/LogStream.h
        base/noncopyable.h
        base/AsyncLogging.cpp
        base/AsyncLogging.h
)
