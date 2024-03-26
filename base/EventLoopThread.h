//
// Created by 15525 on 2024/3/25.
//

#ifndef WEBSER_EVENTLOOPTHREAD_H
#define WEBSER_EVENTLOOPTHREAD_H

#pragma once
#include "../reactor/EventLoop.h"
#include "../base/Condition.h"
#include "../base/MutexLock.h"
#include "../base/Thread.h"
#include "../base/noncopyable.h"
#include "MemoryPool.h"

class EventLoopThread {
public:
    EventLoopThread();
    ~EventLoopThread();

    void start();//线程运行函数

    sp_EventLoop loop_;
    std::unique_ptr<Thread, decltype(deleteElement<Thread>)*> thread_;
};


#endif//WEBSER_EVENTLOOPTHREAD_H
