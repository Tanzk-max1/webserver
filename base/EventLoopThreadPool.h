//
// Created by 15525 on 2024/3/25.
//

#ifndef WEBSER_EVENTLOOPTHREADPOOL_H
#define WEBSER_EVENTLOOPTHREADPOOL_H

#endif//WEBSER_EVENTLOOPTHREADPOOL_H


#pragma once
#include <vector>
#include <memory>
#include "EventLoopThread.h"
#include "../base/Logging.h"
#include "../base/noncopyable.h"

class EventLoopThreadPool {
public:
    EventLoopThreadPool(int numThreads);
    ~EventLoopThreadPool() {LOG << "~EventLoopThreadPool()"; }

    void start();

    sp_EventLoop getNextLoop();

private:
    int numThreads_;
    int index_;
    std::vector<std::shared_ptr<EventLoopThread>> threads_;
};