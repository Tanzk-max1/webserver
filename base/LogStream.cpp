//
// Created by 15525 on 2024/3/17.
//
#include "LogStream.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <algorithm>
#include <limits>

namespace log {
    template class FixedBuffer<kSmallBufferSize>;

    template class FixedBuffer<kLargeBufferSize>;
//
//LogStream& LogStream::operator<<(short v) {
//    *this << static_cast<int>(v);
//    return *this;
//}

template <typename T>
void LogStream::FormatInt(T number) {
    if (buffer_.capacity() >= kMaxNumberSize) {
        size_t size = IntToString(buffer_.current_buffer(),number);
        buffer_.add(size);
    }
}

//重载输出流运算符 bool型
LogStream& LogStream::operator<<(bool express) {
    Write(express ? "1" : "0", 1);
    return *this;
}

//重载输出流运算符 整形
LogStream& LogStream::operator<<(short number) {
    *this << static_cast<int>(number);
    return *this;
}

LogStream& LogStream::operator<<(unsigned short number) {
    *this << static_cast<unsigned int>(number);
    return *this;
}

LogStream& LogStream::operator<<(int number) {
    FormatInt(number);
    return *this;
}

LogStream& LogStream::operator<<(unsigned int number) {
    FormatInt(number);
    return *this;
}

LogStream& LogStream::operator<<(long number) {
    FormatInt(number);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long number) {
    FormatInt(number);
    return *this;
}

LogStream& LogStream::operator<<(long long number) {
    FormatInt(number);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long long number) {
    FormatInt(number);
    return *this;
}

LogStream& LogStream::operator<<(float number) {
    *this << static_cast<double>(number);
    return *this;
}

LogStream& LogStream::operator<<(double number) {
    if (buffer_.capacity() >= kMaxNumberSize) {
        int size = snprintf(buffer_.current_buffer(), kMaxNumberSize, "%.12g", number);
        buffer_.add(size);
    }
    return *this;
}

LogStream& LogStream::operator<<(long double number) {
    if (buffer_.capacity() >= kMaxNumberSize) {
        int size = snprintf(buffer_.current_buffer(), kMaxNumberSize, "%.12Lg", number);
        buffer_.add(size);
    }
    return *this;
}

//重载输出流运算符 字符串
LogStream& LogStream::operator<<(char str) {
    Write(&str, 1);
    return *this;
}

LogStream& LogStream::operator<<(const char* str) {
    if (str) {
        Write(str, strlen(str));
    } else {
        Write("(null)", 6);
    }
    return *this;
}

LogStream& LogStream::operator<<(const unsigned char* str) {
    return operator<<(reinterpret_cast<const char*>(str));
}

LogStream& LogStream::operator<<(const std::string& str) {
    Write(str.c_str(), str.size());
    return *this;
}
}