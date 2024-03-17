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
        base/Thread.cpp
        base/Thread.h
        base/CountDownLatch.h
        base/CountDownLatch.cpp
        base/Coudition.h
        base/MutexLock.h
)
