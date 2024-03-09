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



};