//
// Created by 15525 on 2024/3/8.
//

#include "Channel.h"

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




};