//
// Created by 15525 on 2024/3/17.
//

#ifndef WEBSER_LOGSTREAM_H
#define WEBSER_LOGSTREAM_H

#endif//WEBSER_LOGSTREAM_H


#pragma once
// #pragma once 指令确保该头文件在编译过程中只被包含一次。
#include <assert.h>
#include <string.h>
#include <string>
#include "noncopyable.h"

class AsyncLogging;
const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;

// 固定大小缓冲区的模板类

template <int SIZE>
class FixedBuffer : noncopyable {
public:
    FixedBuffer() : cur_(data_) {}

    ~FixedBuffer() {}
    // 向缓冲区追加数据
    void append(const char* buf, size_t len) {
        if (avail() > static_cast<int>(len)) {
            memcpy(cur_, buf, len);
            cur_ += len;
        }
    }

    // 返回缓冲区的数据指针
    const char* data() const { return data_; }

    // 返回缓冲区的数据长度
    int length() const { return static_cast<int>(cur_ - data_); }

    // 返回当前位置的指针
    char* current() { return cur_; }

    // 返回缓冲区中的可用空间大小
    int avail() const { return static_cast<int>(end() - cur_); }

    // 将当前位置向后移动指定长度
    void add(size_t len) { cur_ += len; }

    // 重置缓冲区状态
    void reset() { cur_ = data_; }

    // 清零缓冲区
    void bzero() { memset(data_, 0, sizeof data_); }


private:
    // 返回缓冲区的末尾指针
    const char* end() const { return data_ + sizeof data_; }

    char data_[SIZE];
    char* cur_;
};

// LogStream 类用于格式化日志消息
class LogStream : noncopyable{
    typedef LogStream self;
public:
    typedef FixedBuffer<kSmallBuffer> Buffer;

    // 不同数据类型的重载运算符
    self& operator<< (bool v) {
        buffer_.append(v ? "1" : "0", 1 );
        return  *this;
    }

    self& operator<< (short);
    self& operator<< (unsigned short);
    self& operator<< (int);
    self& operator<< (unsigned int);
    self& operator<< (long);
    self& operator<< (unsigned long);
    self& operator<< (long long);
    self& operator<< (unsigned long long);
    self& operator<< (const void*);
    self& operator<< (float v) {
            *this << static_cast<double>(v);
            return *this;
    };
    self&operator << (double);
    self&operator << (long double);
    self&operator << (char v) {
            buffer_.append(&v,1);
            return *this;
    };
    self&operator << (const char* str) {
            if (str)
                buffer_.append(str,strlen(str));
            else
                buffer_.append("(null)",6);
            return *this;
    };
    self&operator << (const unsigned char* str){
            // reinterpret_cast⽤在任意指针(或引⽤)类型之间的转换
            return operator<<(reinterpret_cast<const char *>(str));
    };
    self&operator << (const std::string& v){
            buffer_.append(v.c_str(),v.size());
            return *this;
    };
    // 向缓冲区追加数据
    void append(const char* data,int len) {buffer_.append(data,len); }
    // 返回缓冲区
    const Buffer& buffer() const { return buffer_; }

    // 重置缓冲区状态
    void resetBuffer() { buffer_.reset(); }


private:
    // 静态断言的辅助函数
    void staticCheck();
    // 格式化整数并追加到缓冲区
    template <typename T>
    void formatInteger(T);

    Buffer buffer_;

    static const int kMaxNumericSize = 32;

};