//
// Created by 15525 on 2024/3/8.
//
#include "EventLoop.h"
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <iostream>
#include "Util.h"
#include "base/Logging.h"

using namespace std;

EventLoop::EventLoop()
    : looping_(false),
      poller_(new Epoll()),
      wakeupFd_(createEventfd()),
      quit_(false),
      eventHandling_(false),
      callingPendingFunctors_(false),
      threadId_(CurrentThread::tid()),
      pwakeupChannel_(new Channel(this, wakeupFd_)) {
    if (t_loopInThisThread) {
        // LOG << "Another EventLoop " << t_loopInThisThread << " exists in this
        // thread " << threadId_;
    } else {
        t_loopInThisThread = this;
    }
    // pwakeupChannel_->setEvents(EPOLLIN | EPOLLET | EPOLLONESHOT);
    pwakeupChannel_->setEvents(EPOLLIN | EPOLLET);
    pwakeupChannel_->setReadHandler(bind(&EventLoop::handleRead, this));
    pwakeupChannel_->setConnHandler(bind(&EventLoop::handleConn, this));
    poller_->epoll_add(pwakeupChannel_, 0);
}

void EventLoop::Loop() {
    assert(!is_looping_);
    assert(is_in_loop_thread());
    is_looping_ = true;
    is_stop_ = false;
    // LOG_TRACE << "EventLoop " << this << " start looping";

    while (!is_stop_) {
        // 1、epoll_wait阻塞 等待就绪事件
        auto ready_channels = poller_->Poll();
        is_event_handling_ = true;
        // 2、处理每个就绪事件(不同channel绑定了不同的callback)
        for (auto& channel : ready_channels) {
            channel->HandleEvents();
        }
        is_event_handling_ = false;
        PerformPendingFunctions();// 3、执⾏正在等待的函数(fd注册到epoll内核事件表)
        poller_->HandleExpire();//// 4、处理超时事件 到期了就从定时器⼩根堆中删除(定时器析构会EpollDel掉fd)
    }
    is_looping_ = false;
}