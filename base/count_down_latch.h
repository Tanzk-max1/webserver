//
// Created by 15525 on 2024/3/26.
//

#ifndef WEBSER_COUNT_DOWN_LATCH_H
#define WEBSER_COUNT_DOWN_LATCH_H

#include "noncopyable.h"
#include "locker/mutex_lock.h"

namespace utility {
    // CountDownLatch的主要作用是确保Thread中传进去的func真的启动了以后 外层的start才返回
    class CountDownLatch : NonCopyAble {
    public:
        explicit CountDownLatch(int count)
            : mutex_(),
              condition_(mutex_),
              count_(count) {
        }

        ~CountDownLatch() {
        }

        void wait() {
            locker::LockGuard lock(mutex_);
            while (count_ > 0) {
                condition_.wait();
            }
        }

        void count_down() {
            locker::LockGuard lock(mutex_);
            --count_;
            if (count_ == 0) {
                condition_.notify_all();
            }
        }

    private:
        mutable locker::MutexLock mutex_;
        locker::ConditionVariable condition_;
        int count_;
    };

}  // namespace utility



#endif//WEBSER_COUNT_DOWN_LATCH_H
