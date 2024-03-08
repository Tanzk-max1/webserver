//
// Created by 15525 on 2024/3/8.
//

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
    void RunInLoop(Function&& func);
    void QueueInLoop(Function&& func);


};