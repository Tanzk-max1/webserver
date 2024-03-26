//
// Created by 15525 on 2024/3/25.
//

#ifndef WEBSER_EVENTLOOPTHREAD_H
#define WEBSER_EVENTLOOPTHREAD_H

#pragma once
#include "D:\project\webser\EventLoop.h"

#include "../base/MutexLock.h"
#include "../base/Thread.h"
#include "../base/noncopyable.h"
#include "MemoryPool.h"

//class EventLoopThread {
//public:
//    EventLoopThread();
//    ~EventLoopThread();
//
//    void start();//线程运行函数
//
//    sp_EventLoop loop_;
////    std::unique_ptr<Thread, decltype(deleteElement<Thread>)*> thread_;
//private:
//    void threadFunc();
//    //线程运行函数
//
//    sp_EventLoop loop_;
//    std::unique_ptr<Thread, decltype(deleteElement<Thread>)*> thread_;
//
//};

namespace event {
    //IO线程
    class EventLoopThread : utility::NonCopyAble {
    public:
        EventLoopThread();
        ~EventLoopThread();

        //执行线程函数, 在线程函数还没运行起来的时候 一直阻塞 运行起来了才唤醒返回该event_loop
        EventLoop* StartLoop();

    private:
        //线程函数(就是EventLoop的Loop函数)
        void Worker();

    private:
        EventLoop* sub_loop_;    //子loop
        thread::Thread thread_;  //线程对象
        mutable locker::MutexLock mutex_;
        locker::ConditionVariable condition_;
        bool is_exiting_;
    };

}




#endif//WEBSER_EVENTLOOPTHREAD_H
