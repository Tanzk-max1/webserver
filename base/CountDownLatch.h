//
// Created by 15525 on 2024/3/17.
//

#ifndef WEBSER_COUNTDOWNLATCH_H
#define WEBSER_COUNTDOWNLATCH_H



#pragma once
//#include "Condition.h"
#include "MutexLock.h"
#include "noncopyable.h"

// CountDownLatch的主要作用是确保Thread中传进去的func真的启动了以后
// 外层的start才返回
class CountDownLatch : noncopyable {
public:
    explicit CountDownLatch(int count);
    void wait();
    void countDown();

private:
    mutable MutexLock mutex_;
    Condition condition_;
    int count_;
};

#endif//WEBSER_COUNTDOWNLATCH_H