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

namespace log {
    // 类的前置声明
    class AsyncLogging;
    constexpr int kSmallBufferSize = 4000;
    constexpr int kLargeBufferSize = 4000 * 1000;

    // 固定的缓冲区
    template <int buffer_size>
    class FixedBuffer : utility::NonCopyAble {
    public:
        FixedBuffer()
            : size_(0) {
            bzero();
            current_buffer_ = buffer_;
        }

        ~FixedBuffer() {
        }

        //copy buffer的size个字节到current_buffer_来
        void write(const char* buffer, int size) {
            if (capacity() > size) {
                memcpy(current_buffer_, buffer, size);
                this->add(size);
            }
        }

        //源buffer
        const char* buffer() const {
            return buffer_;
        }

        //当前buffer(buffer+偏移量)
        char* current_buffer() {
            return current_buffer_;
        }

        //当前buufer的偏移量（相对buffer起始地址的偏移量）
        int size() const {
            return size_;
        }

        //剩余buffer偏移量 = 总buffer的偏移量 - 当前buffer的偏移量
        int capacity() const {
            return buffer_size - size_;
        }

        //当前buffer偏移size个字节
        void add(size_t size) {
            current_buffer_ += size;
            size_ += size;
        }

        //将当前buffer偏移量置为0,即为buffer
        void reset() {
            current_buffer_ = buffer_;
            size_ = 0;
        }

        //给buffer置0
        void bzero() {
            memset(buffer_, 0, sizeof(buffer_));
        }

    private:
        char buffer_[buffer_size];
        char* current_buffer_;
        int size_;
    };

    //输出流对象 重载输出流运算符<<  将值写入buffer中
    class LogStream : utility::NonCopyAble {
    public:
        using Buffer = FixedBuffer<kSmallBufferSize>;

        LogStream() {
        }
        ~LogStream() {
        }

        //重载输出流运算符<<  将值写入buffer中
        LogStream& operator<<(bool express);

        LogStream& operator<<(short number);
        LogStream& operator<<(unsigned short);
        LogStream& operator<<(int);
        LogStream& operator<<(unsigned int);
        LogStream& operator<<(long);
        LogStream& operator<<(unsigned long);
        LogStream& operator<<(long long);
        LogStream& operator<<(unsigned long long);

        LogStream& operator<<(float number);
        LogStream& operator<<(double);
        LogStream& operator<<(long double);

        LogStream& operator<<(char str);
        LogStream& operator<<(const char*);
        LogStream& operator<<(const unsigned char*);
        LogStream& operator<<(const std::string&);

        void Write(const char* buffer, int size) {
            buffer_.write(buffer, size);
        }

        const Buffer& buffer() const {
            return buffer_;
        }

        void reset_buffer() {
            buffer_.reset();
        }

    private:
        template <typename T>
        void FormatInt(T);

    private:
        static constexpr int kMaxNumberSize = 32;

        Buffer buffer_;
    };

}  // namespace log

#endif  // LOG_LOG_STREAM_H_


//class AsyncLogging;
//const int kSmallBufferSizeSize = 4000;
//const int kLargeBufferSize = 4000 * 1000;
//
//// 固定大小缓冲区的模板类
//
//template <int SIZE>
//class FixedBuffer : noncopyable {
//public:
//    FixedBuffer() : cur_(data_) {}
//
//    ~FixedBuffer() {}
//    // 向缓冲区追加数据
//    void append(const char* buf, size_t len) {
//        if (avail() > static_cast<int>(len)) {
//            memcpy(cur_, buf, len);
//            cur_ += len;
//        }
//    }
//
//    // 返回缓冲区的数据指针
//    const char* data() const { return data_; }
//
//    // 返回缓冲区的数据长度
//    int length() const { return static_cast<int>(cur_ - data_); }
//
//    // 返回当前位置的指针
//    char* current() { return cur_; }
//
//    // 返回缓冲区中的可用空间大小
//    int avail() const { return static_cast<int>(end() - cur_); }
//
//    // 将当前位置向后移动指定长度
//    void add(size_t len) { cur_ += len; }
//
//    // 重置缓冲区状态
//    void reset() { cur_ = data_; }
//
//    // 清零缓冲区
//    void bzero() { memset(data_, 0, sizeof data_); }
//
//
//private:
//    // 返回缓冲区的末尾指针
//    const char* end() const { return data_ + sizeof data_; }
//
//    char data_[SIZE];
//    char* cur_;
//};
//
//// LogStream 类用于格式化日志消息
//class LogStream : noncopyable{
//    typedef LogStream self;
//public:
//    typedef FixedBuffer<kSmallBuffer> Buffer;
//
//    // 不同数据类型的重载运算符
//    self& operator<< (bool v) {
//        buffer_.append(v ? "1" : "0", 1 );
//        return  *this;
//    }
//
//    self& operator<< (short);
//    self& operator<< (unsigned short);
//    self& operator<< (int);
//    self& operator<< (unsigned int);
//    self& operator<< (long);
//    self& operator<< (unsigned long);
//    self& operator<< (long long);
//    self& operator<< (unsigned long long);
//    self& operator<< (const void*);
//    self& operator<< (float v) {
//            *this << static_cast<double>(v);
//            return *this;
//    };
//    self&operator << (double);
//    self&operator << (long double);
//    self&operator << (char v) {
//            buffer_.append(&v,1);
//            return *this;
//    };
//    self&operator << (const char* str) {
//            if (str)
//                buffer_.append(str,strlen(str));
//            else
//                buffer_.append("(null)",6);
//            return *this;
//    };
//    self&operator << (const unsigned char* str){
//            // reinterpret_cast⽤在任意指针(或引⽤)类型之间的转换
//            return operator<<(reinterpret_cast<const char *>(str));
//    };
//    self&operator << (const std::string& v){
//            buffer_.append(v.c_str(),v.size());
//            return *this;
//    };
//    // 向缓冲区追加数据
//    void append(const char* data,int len) {buffer_.append(data,len); }
//    // 返回缓冲区
//    const Buffer& buffer() const { return buffer_; }
//
//    // 重置缓冲区状态
//    void resetBuffer() { buffer_.reset(); }
//
//
//private:
//    // 静态断言的辅助函数
//    void staticCheck();
//    // 格式化整数并追加到缓冲区
//    template <typename T>
//    void formatInteger(T);
//
//    Buffer buffer_;
//
//    static const int kMaxNumericSize = 32;
//
//};