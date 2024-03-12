//
// Created by 15525 on 2024/3/12.
//
#include "Epoll.h"
#include <assert.h>
#include <errno.h>size
#include <netinet/in.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <deque>
#include <queue>
#include "Util.h"
#include "base/Logging.h"


#include <arpa/inet.h>
#include <iostream>
using namespace std;

void Epoll::poll(std::vector<sp_Channel>& req){
    int event_count = Epoll_wait(epollFd_, &*events_.begin(),events_size(),EPOLLWAIT_TIME);
    for (int i = 0; i < event_count; ++i) {
        int fd = events_[i].data.fd;
        sp_Channel temp = channelMap_[fd];
        temp->setRevents(events_[i].events);
        req.emplace_back(std::move(temp));
    }// LOG << "Epoll finished";
}