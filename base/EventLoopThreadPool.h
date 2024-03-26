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
#include "Logging.h"
#include "noncopyable.h"


//EventLoopThreadPool 类的构造函数接受一个指向主 EventLoop 对象的指针和线程数量作为参数，并初始化成员变量。
//析构函数 ~EventLoopThreadPool() 输出调试日志。
// Start() 函数用于在主线程中启动线程池，创建 EventLoopThread 对象并获取对应的 EventLoop 对象，将其存储在 sub_loops_ 数组中。
// GetNextLoop() 函数用于从已经开始事件循环的 EventLoop 对象列表中获取一个 EventLoop 对象。
//  如果还没有开始事件循环的对象，将返回主 EventLoop 对象。该函数通过循环选择下一个 EventLoop 对象，并使用取余运算符实现循环选择

namespace event {
    // IO线程池
    EventLoopThreadPool::EventLoopThreadPool(EventLoop* main_loop, int thread_num)
        : main_loop_(main_loop),
          thread_num_(thread_num),
          is_started_(false),
          next_(0) {
        // 检查线程数量是否合法
        if (thread_num_ <= 0) {
            LOG(FATAL) << "thread num <= 0";
        }
        // 预分配空间以提高性能
        sub_loop_threads_.reserve(thread_num_);
        sub_loops_.reserve(thread_num_);
    }

    EventLoopThreadPool::~EventLoopThreadPool() {
        LOG(DEBUG) << "~EventLoopThreadPool()";
    }

    // 主线程（主Loop对象）创建event_loop线程池
    void EventLoopThreadPool::Start() {
        assert(main_loop_->is_in_loop_thread());
        is_started_ = true;

        // 创建event_loop_thread_pool，并将开始Loop事件循环的EventLoop对象存入数组中
        for (int i = 0; i < thread_num_; ++i) {
            auto event_loop_thread = std::make_shared<EventLoopThread>();
            sub_loop_threads_.push_back(event_loop_thread);
            sub_loops_.push_back(event_loop_thread->StartLoop());
        }
    }

    // 从已经开始Loop事件循环的EventLoop对象列表中返回一个EventLoop对象，如果此时还没有，则返回主Loop
    EventLoop* EventLoopThreadPool::GetNextLoop() {
        // 调用这个函数的必须是主Loop线程
        assert(main_loop_->is_in_loop_thread());
        assert(is_started_);

        // 如果此时还没有开始Loop的EventLoop对象，就返回主Loop
        auto event_loop = main_loop_;
        if (!sub_loops_.empty()) {
            event_loop = sub_loops_[next_];
            next_ = (next_ + 1) % thread_num_;
        }

        return event_loop;
    }

}  // namespace event


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