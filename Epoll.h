//
// Created by 15525 on 2024/3/12.
//

#ifndef WEBSER_EPOLL_H
#define WEBSER_EPOLL_H


#pragma once
#include "Channel.h"
#include "../connection/httpConnection.h"
#include "Timer.h"
#include <sys/epoll.h>
#include <memory>
#include <unordered_map>
#include <vector>

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

typedef std::shared_ptr<Epoll> sp_Epoll;


#endif//WEBSER_EPOLL_H
