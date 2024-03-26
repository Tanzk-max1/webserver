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
    //定义小BufferSize和大BufferSize的固定缓冲区类
    template class FixedBuffer<kLargeBufferSize>;
//
//LogStream& LogStream::operator<<(short v) {
//    *this << static_cast<int>(v);
//    return *this;
//}

    //用于format int
    static const char digits[] = "9876543210123456789";
    static const char* zero = digits + 9;

    template <typename T>
    static size_t IntToString(char buffer[], T number) {
        char* buf = buffer;

        // 从数字的个位开始，逐个取出每个位上的数字并转换为字符
        do {
            int index = static_cast<int>(number % 10);
            number /= 10;
            *buf++ = zero[index];  // 将对应位上的数字转换为字符并存入缓冲区
        } while (number != 0);

        // 如果这个值是负数，就在缓冲区最后加上负号
        if (number < 0) {
            *buf++ = '-';
        }
        *buf = '\0';

        // 由于先从个位开始存储，需要将缓冲区中的字符反转（从高位到低位）
        std::reverse(buffer, buf);

        // 返回转换后的字符串的长度
        return buf - buffer;
    }
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