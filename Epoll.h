//
// Created by 15525 on 2024/3/12.
//

#ifndef WEBSER_EPOLL_H
#define WEBSER_EPOLL_H


#pragma once
#include <sys/epoll.h>
#include <memory>
#include <unordered_map>
#include <vector>
#include "Channel.h"
#include "HttpData.h"
#include "Timer.h"

class Epoll {
public:
    Epoll();
    ~Epoll();

    void epoll_add(const sp_Channel & request);
    void epoll_mod(const sp_Channel & request);
    void epoll_del(const sp_Channel & request);
    void epoll_poll(std::vector<sp_Channel>& req);

private:
    int epollFd_;
    std::vector<epoll_event> events_;// epoll_wait()返回的活动事件都放在这个数组里
    std::unordered_map<int,sp_Channel> channelMap_;
};



#endif//WEBSER_EPOLL_H
