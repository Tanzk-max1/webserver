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
        base/LogFile.cpp
        base/LogFile.h
        base/FileUtil.cpp
        base/FileUtil.h
        base/Logging.cpp
        base/Logging.h
        base/MemoryPool.h
        base/MemoryPool.cpp
        test/test_MemoryPool.cpp
        base/EventLoopThreadPool.h
        base/EventLoopThread.h
        base/count_down_latch.h
        base/EventLoopThread.cpp
        base/http_connection.cpp
        base/http_type.cpp
        base/http_type.h
        base/http_connection.h
        base/timer.h
        base/timer.cpp
)
