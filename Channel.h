//
// Created by 15525 on 2024/3/8.
//
#pragma once
#include <sys/epoll.h>
#include <sys/epoll.h>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include "Timer.h"

class EventLoop;
class HttpData;


#ifndef WEBSER_CHANNEL_H
#define WEBSER_CHANNEL_H

#endif//WEBSER_CHANNEL_H

class Channel {
public:
    typedef std::function<void()> EventCallBack;

    Channel();
    explicit Channel(int fd);
    -Channel();

    //IO事件回调函数的调用接口
    // EventLoop中调⽤Loop开始事件循环 会调⽤Poll得到就绪事件
    // 然后依次调⽤此函数处理就绪事件

    void HandleEvents();
    void HandleRead();  // 处理读事件的回调

    void HandleWrite();     // 处理写事件的回调
    void HandleUpdate();     // 处理更新事件的回调
    void HandleError();      // 处理错误事件的回调
    int get_fd();
    void set_fd(int fd);

    //返回weak_ptr 所指向的shared_ptr对象
    std::shared_ptr<http::HttpConnection> holder();
    void set_holder(std::shared_ptr<http::HttpConnection> holder);

    void set_read_handler(EventCallBack&& read_headler);
    void set_white_handler(EventCallBack&& white_headler);
    void set_update_handler(EventCallBack&& update_headler);
    void set_error_handler(EventCallBack&& error_headler);

    void set_revents(int revents);
    int& events();
    void set_events(int events);
    int last_events();
    bool update_last_events();

private:
    int fd_; //  Channel的文件描述符
    int events_;//  Channel正在监听的事件，关注的事件
    int revents_;// 返回的就绪事件。实际发生的事件
    int last_events_;// 上一次的事件

    // weak_ptr是⼀个观测者（不会增加或减少引⽤计数）,同时也没有重载->,和*等运算符 所以不能直接使⽤
    // 可以通过lock函数得到它的shared_ptr（对象没销毁就返回，销毁了就返回空shared_ptr）
    // expired函数判断当前对象是否销毁了
    std::weak_ptr<http::HttpConnection> holder_;

    EventCallBack read_handler_;
    EventCallBack write_handler_;
    EventCallBack update_handler_;
    EventCallBack error_handler_;

};

void Channel::HandleEvents() {
    events_ = 0;
    //触发挂起时间，并且没触发可读事件
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))//EPOLLIN（可读事件）和 EPOLLOUT（可写事件),EPOLLHUP是挂起事件
        //发生了挂起事件并且没有发生可读事件
    {
        events_ = 0;//
        return;
    }
    //触发错误事件
    if (revents_ & EPOLLERR){
        HandleError();
        events_ = 0;
        return;
    }
    //触发刻度时间|高优先级可读|对端（客户端）关闭连接
    if (revents_ & (EPOLLIN | EPOLLPRI | RPOLLRDHUP)){
        HandleError();

    }

    //触发可写事件
    if (revents_ & EROLLOUT){
        HandleWrite();

    }

    //处理更新监听事件(EpollMod)
    HandleUpdate();

}