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
    : flushInterval_(flushInterval),
      running_(false),
      basename_(logFileName_),
      thread_(std::bind(&AsyncLogging::threadFunc, this), "Logging"),
      mutex_(),
      cond_(mutex_),
      currentBuffer_(new Buffer),
      nextBuffer_(new Buffer),
      buffers_(),
      latch_(1) {
    assert(logFileName_.size() > 1);
    currentBuffer_->bzero();
    nextBuffer_->bzero();
    buffers_.reserve(16);
}

void AsyncLogging::append(const char* logline, int len) {
    MutexLockGuard lock(mutex_);
    if (currentBuffer_->avail() > len)
        currentBuffer_->append(logline, len);
    else {
        buffers_.push_back(currentBuffer_);
        currentBuffer_.reset();
        if (nextBuffer_)
            currentBuffer_ = std::move(nextBuffer_);
        else
            currentBuffer_.reset(new Buffer);
        currentBuffer_->append(logline, len);
        cond_.notify();
    }
}

void AsyncLogging::threadFunc() {
    assert(running_ == true);
    latch_.countDown();
    LogFile output(basename_);//LogFile用于将日志写入文件
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferVector buffersToWrite;//该vector属于后端线程，用于和前端的buffer进行交换
    buffersToWrite.reserve(16);
    while (running_) {
        assert(newBuffer1 && newBuffer1->length() == 0);
        assert(newBuffer2 && newBuffer2->length() == 0);
        assert(buffersToWrite.empty());

        {
            MutexLockGuard lock(mutex_);
            if (buffers_.empty())  // unusual usage!
            {
                cond_.waitForSeconds(flushInterval_);
            }
            buffers_.push_back(currentBuffer_);
            currentBuffer_.reset();

            currentBuffer_ = std::move(newBuffer1);
            buffersToWrite.swap(buffers_);
            if (!nextBuffer_) {
                nextBuffer_ = std::move(newBuffer2);
            }
        }

        assert(!buffersToWrite.empty());
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
            output.append(buffersToWrite[i]->data(), buffersToWrite[i]->length());
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
            newBuffer2 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }

        buffersToWrite.clear();
        output.flush();
    }
    output.flush();
}
