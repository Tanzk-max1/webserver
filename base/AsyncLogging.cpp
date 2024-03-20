//
// Created by 15525 on 2024/3/17.
//
#include "AsyncLogging.h"
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <functional>
#include "LogFile.h"

AsyncLogging::AsyncLogging(std::string logFileName_, int flushInterval)
    : flushInterval_(flushInterval), // 刷新间隔
      running_(false), // 运行状态标志
      basename_(logFileName_), // 日志文件名的基本部分
      thread_(std::bind(&AsyncLogging::threadFunc, this), "Logging"), // 创建一个线程对象，绑定线程函数
      mutex_(), // 互斥锁对象
      cond_(mutex_), // 条件变量对象
      currentBuffer_(new Buffer), // 当前缓冲区指针，新建一个缓冲区对象
      nextBuffer_(new Buffer), // 下一个缓冲区指针，新建一个缓冲区对象
      buffers_(), // 缓冲区列表
      latch_(1) { // 启动标志，初始值为1
    assert(logFileName_.size() > 1); // 断言：日志文件名长度大于1
    currentBuffer_->bzero(); // 清空当前缓冲区内容
    nextBuffer_->bzero(); // 清空下一个缓冲区内容
    buffers_.reserve(16); // 预留16个缓冲区的空间
}


void AsyncLogging::append(const char* logline, int len) {
    MutexLockGuard lock(mutex_); // 加锁以确保线程安全

    // 检查当前缓冲区是否有足够的空间来追加日志行
    if (currentBuffer_->avail() > len)
        currentBuffer_->append(logline, len); // 将日志行追加到当前缓冲区
    else {
        buffers_.push_back(currentBuffer_); // 将当前缓冲区添加到缓冲区列表
        currentBuffer_.reset(); // 重置当前缓冲区指针

        // 检查是否有可用的下一个缓冲区
        if (nextBuffer_)
            currentBuffer_ = std::move(nextBuffer_); // 将下一个缓冲区移动到当前缓冲区
        else
            currentBuffer_.reset(new Buffer); // 创建一个新的缓冲区并将其分配给当前缓冲区指针

        currentBuffer_->append(logline, len); // 将日志行追加到新的当前缓冲区
        cond_.notify(); // 通知等待的线程有可用的缓冲区供写入
    }
}

void AsyncLogging::threadFunc() {
    assert(running_ == true); // 断言：确保 running_ 为 true
    latch_.countDown(); // 减少启动标志的计数

    LogFile output(basename_); // 日志文件对象，用于将日志写入文件

    BufferPtr newBuffer1(new Buffer); // 新的缓冲区指针1，新建一个缓冲区对象
    BufferPtr newBuffer2(new Buffer); // 新的缓冲区指针2，新建一个缓冲区对象
    newBuffer1->bzero(); // 清空新缓冲区1的内容
    newBuffer2->bzero(); // 清空新缓冲区2的内容
    BufferVector buffersToWrite;//该vector属于后端线程，用于和前端的buffer进行交换
    buffersToWrite.reserve(16);//预留16个缓冲区的空间
    while (running_) {
        assert(newBuffer1 && newBuffer1->length() == 0); // 断言：确保新缓冲区1存在且长度为0
        assert(newBuffer2 && newBuffer2->length() == 0); // 断言：确保新缓冲区2存在且长度为0
        assert(buffersToWrite.empty()); // 断言：确保待写入缓冲区列表为空

        {
            MutexLockGuard lock(mutex_); // 加锁以确保线程安全

            if (buffers_.empty()) {
                cond_.waitForSeconds(flushInterval_); // 等待一段时间，直到有可用的缓冲区或超时
            }

            buffers_.push_back(currentBuffer_); // 将当前缓冲区添加到缓冲区列表
            currentBuffer_.reset(); // 重置当前缓冲区指针

            currentBuffer_ = std::move(newBuffer1); // 将新缓冲区1移动到当前缓冲区
            buffersToWrite.swap(buffers_); // 交换待写入的缓冲区列表和缓冲区列表

            if (!nextBuffer_) {
                nextBuffer_ = std::move(newBuffer2); // 如果下一个缓冲区为空，则将新缓冲区2移动到下一个缓冲区
            }
        }

        assert(!buffersToWrite.empty());// 断言：确保待写入缓冲区列表不为空
//如果队列中buffer数⽬⼤于25，就删除多余数据
        //避免日志堆积：前端日志记录过快，后端来不及写入文件
        if (buffersToWrite.size() > 25) {
            // char buf[256];
            // snprintf(buf, sizeof buf, "Dropped log messages at %s, %zd larger
            // buffers\n",
            //          Timestamp::now().toFormattedString().c_str(),
            //          buffersToWrite.size()-2);
            // fputs(buf, stderr);
            // output.append(buf, static_cast<int>(strlen(buf)));
            //TODO:删除数据是加错误提示
            //只留原始的两个buffer，其余的删除
            buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
        }

        for (size_t i = 0; i < buffersToWrite.size(); ++i) {
            // FIXME: use unbuffered stdio FILE ? or use ::writev ?
            output.append(buffersToWrite[i]->data(), buffersToWrite[i]->length());// 将待写入缓冲区的数据追加到日志文件
        }

        //重新调整buffertowhrite的大小，仅保留两个原始buffer
        if (buffersToWrite.size() > 2) {
            // drop non-bzero-ed buffers, avoid trashing
            buffersToWrite.resize(2);
        }

        if (!newBuffer1) {
            assert(!buffersToWrite.empty());
            newBuffer1 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }
        //因为非空指针的话是true，然后！new得到的是true，说明原来的那个是空的
        //首先，代码使用 if 语句检查 newBuffer1 是否为空指针，即是否指向 nullptr。当 newBuffer1 为空时，执行 if 语句块中的代码。
        //在 if 语句块中，第一行使用了 assert 断言来验证一个条件，即 buffersToWrite 是否为空。assert 断言用于在代码中插入调试断点，用于确保某个条件为真，如果条件为假，则触发断言失败，输出错误信息并终止程序。
        //如果 buffersToWrite 不为空，则继续执行下面的代码。第三行将 newBuffer1 的指针指向 buffersToWrite 中的最后一个元素，即 buffersToWrite.back()。然后，通过 pop_back() 函数将 buffersToWrite 中的最后一个元素移除。
        //最后一行调用 newBuffer1 的 reset() 函数，对其进行重置操作。reset() 函数的具体含义可能根据代码上下文而有所不同，但通常用于清空或重置对象的状态或成员变量。

        if (!newBuffer2) {
            assert(!buffersToWrite.empty());
            newBuffer2 = buffersToWrite.back(); // 将待写入缓冲区列表的最后一个元素赋值给新缓冲区2
            buffersToWrite.pop_back(); // 移除待写入缓冲区列表的最后一个元素
            newBuffer2->reset(); // 重置新缓冲区2
        }

        buffersToWrite.clear(); // 清空待写入缓冲区列表
        output.flush(); // 刷新日志文件的缓冲区，将数据写入文件
    }

    output.flush(); // 在线程结束前再次刷新日志文件的缓冲区，确保所有数据都被写入文件
}