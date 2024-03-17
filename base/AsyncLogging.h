//
// Created by 15525 on 2024/3/17.
//

#ifndef WEBSER_ASYNCLOGGING_H
#define WEBSER_ASYNCLOGGING_H

#endif//WEBSER_ASYNCLOGGING_H


// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#pragma once
#include <functional>
#include <string>
#include <vector>
#include "CountDownLatch.h"
#include "LogStream.h"
#include "MutexLock.h"
#include "Thread.h"
#include "noncopyable.h"


class AsyncLogging : noncopyable {
public:
    AsyncLogging(const std::string basename, int flushInterval = 2);
    ~AsyncLogging() {
        if (running_) stop();
    }
    void append(const char* logline, int len);

    void start() {
        running_ = true;
        thread_.start();
        latch_.wait();
    }

    void stop() {
        running_ = false;
        cond_.notify();
        thread_.join();
    }

private:
    void threadFunc(); //后端⽇志线程函数，⽤于把缓冲区⽇志写⼊⽂件
    const int flushInterval_;//超时时间，每隔⼀段时间写⽇志
    bool running_; ///FIXME:std::atomic<bool> running_;
    const std::string basename_;    //日志名字
    Thread thread_;//后端线程，用于将日志写入文件

    std::string basename_;
    Thread thread_;
    typedef FixedBuffer<kLargeBuffer> Buffer;
    typedef std::vector<std::shared_ptr<Buffer>> BufferVector;
    typedef std::shared_ptr<Buffer> BufferPtr;

    MutexLock mutex_;
    Condition cond_;
    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;
    BufferVector buffers_;
    CountDownLatch latch_; //倒计时，用于指示日志记录器何时开始工作
};