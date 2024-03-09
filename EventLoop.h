//
// Created by 15525 on 2024/3/8.
//

#pragma once
#include <functional>
#include <memory>
#include <vector>
#include "Channel.h"
#include "Epoll.h"
#include "Util.h"
#include "base/CurrentThread.h"
#include "base/Logging.h"
#include "base/Thread.h"


#ifndef WEBSER_EVENTLOOP_H
#define WEBSER_EVENTLOOP_H

#endif//WEBSER_EVENTLOOP_H

class EventLoop{
public:
    typedef std::function<void> Function;
    EventLoop();
    ~EventLoop();

    void Loop();// 开始事件循环 调⽤该函数的线程必须是该 EventLoop 所在线程，也就是 Loop 函数不能跨线程调⽤
    void StopLoop();//停止Loop
    void RunInLoop(Function&& func);//如果当前线程就是创建此EventLoop的线程 就调⽤callback(关闭连接 EpollDel) 否则就放⼊等待执⾏函数区
    void QueueInLoop(Function&& func);// 把此函数放⼊等待执⾏函数区 如果当前是跨线程 或者正在调⽤等待的函数则唤醒
    void PollerAdd(std::shared_ptr<Channel> channel,int timeout = 0);// 把fd和绑定的事件注册到epoll内核事件表
    void PollerMod(std::shared_ptr<Channel> channel,int timeout = 0);// 在epoll内核事件表修改fd所绑定的事件
    void PollerDel(std::shared_ptr<Channel> channer);//从epool内核事件表中删除fd及其绑定的时间
    void ShutDown(std::shared_ptr<Channel> channer);//只是关闭链接
    bool is_in_loop_thread();

private:
    static int CreateEventfd();// 创建eventfd 类似管道的 进程间通信⽅式
    void HandleRead();// eventfd的读回调函数(因为event_fd写了数据，所以触发可读事件，从event_fd读数据)
    void HandleUpdate();// eventfd的更新事件回调函数(更新监听事件)
    void WakeUp();//异步唤醒SubLoop的epoll_wait(向event_fd中写入数据）
    void PerformPendingFunctions(); //执⾏正在等待的函数(SubLoop注册EpollAdd连接套接字以及绑定时间的函数

private:
    std::shared_ptr<Poller> poller_;// io多路复⽤ 分发器
    int event_fd_;
    // ⽤于异步唤醒 SubLoop 的 Loop 函数中的Poll(epoll_wait因为还没有注册fd会⼀直阻塞)
    std::shared_ptr<Channel> wakeup_channel_;  // ⽤于异步唤醒的 channel
    pid_t thread_id_;
    // 线程id
    mutable locker::MutexLock mutex_;
    std::vector<Function> pending_functions_;  // 正在等待处理的函数
    bool is_stop_; //是否停止事件循环
    bool is_looping_; // 是否正在事件循环
    bool is_event_handling_; // 是否正在处理事件
    bool is_calling_pending_functions_; //是否正在调用等待处理的数据


};