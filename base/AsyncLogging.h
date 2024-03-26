//
// Created by 15525 on 2024/3/17.
//

#ifndef WEBSER_ASYNCLOGGING_H
#define WEBSER_ASYNCLOGGING_H


// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#pragma once
#include <functional>
#include <string>
#include <vector>
#include "CountDownLatch.h"
#include "LogStream.h"
#include "MutexLock.h"
#include "Thread.h"
#include "noncopyable.h"


//class AsyncLogging : noncopyable {
//public:
//    AsyncLogging(const std::string basename, int flushInterval = 2);
//    ~AsyncLogging() {
//        if (running_) stop();
//    }
//    void append(const char* logline, int len);
//
//    void start() {
//        running_ = true;
//        thread_.start();
//        latch_.wait();
//    }
//
//    void stop() {
//        running_ = false;
//        cond_.notify();
//        thread_.join();
//    }
//
//private:
//    void threadFunc(); //后端⽇志线程函数，⽤于把缓冲区⽇志写⼊⽂件
//    const int flushInterval_;//超时时间，每隔⼀段时间写⽇志
//    bool running_; ///FIXME:std::atomic<bool> running_;
//    const std::string basename_;    //日志名字
//    Thread thread_;//后端线程，用于将日志写入文件
//
//    std::string basename_;
//    Thread thread_;
//    typedef FixedBuffer<kLargeBuffer> Buffer;
//    typedef std::vector<std::shared_ptr<Buffer>> BufferVector;
//    typedef std::shared_ptr<Buffer> BufferPtr;
//
//    MutexLock mutex_;
//    Condition cond_;
//    BufferPtr currentBuffer_;
//    BufferPtr nextBuffer_;
//    BufferVector buffers_;
//    CountDownLatch latch_; //倒计时，用于指示日志记录器何时开始工作
//};


namespace log {
    // 操作文件的工具类
    class FileUtils {
    public:
        explicit FileUtils(std::string file_name)
            : fp_(fopen(file_name.c_str(), "ae")) {
            setbuffer(fp_, buffer_, sizeof(buffer_));
        }

        ~FileUtils() {
            fclose(fp_);
        }

        // 写日志到文件
        void Write(const char* single_log, const size_t size) {
            size_t write_size = fwrite_unlocked(single_log, 1, size, fp_);
            // 剩余size
            size_t remain_size = size - write_size;
            //如果一次没写完 就继续写
            while (remain_size > 0) {
                size_t bytes = fwrite_unlocked(single_log + write_size, 1, remain_size, fp_);
                if (bytes == 0) {
                    if (ferror(fp_)) {
                        perror("Write to log file failed");
                    }
                    break;
                }
                write_size += bytes;
                remain_size = size - write_size;
            }
        }

        void Flush() {
            fflush(fp_);
        }

    private:
        FILE* fp_;
        char buffer_[64 * 1024];
    };

    // 日志文件 封装了FileUtils
    class LogFile : utility::NonCopyAble {
    public:
        // 每写flush_every_n次，就会flush一次
        LogFile(const std::string& file_name, int flush_every_n = 1024)
            : file_name_(file_name),
              flush_every_n_(flush_every_n),
              count_(0),
              mutex_() {
            file_.reset(new FileUtils(file_name));
        }

        ~LogFile() {
        }

        // 写日志到文件
        void Write(const char* single_log, int size) {
            locker::LockGuard lock(mutex_);
            {
                // 每写flush_every_n次，就会flush一次
                file_->Write(single_log, size);
                ++count_;
                if (count_ >= flush_every_n_) {
                    count_ = 0;
                    file_->Flush();
                }
            }
        }

        void Flush() {
            locker::LockGuard lock(mutex_);
            {
                file_->Flush();
            }
        }

    private:
        const std::string file_name_;
        const int flush_every_n_;
        int count_;
        locker::MutexLock mutex_;
        std::unique_ptr<FileUtils> file_;
    };

    class AsyncLogging : utility::NonCopyAble {
        using Buffer = FixedBuffer<kLargeBufferSize>;
    public:
        AsyncLogging(const std::string file_name, int flush_interval = 2);
        ~AsyncLogging();

        void WriteLog(const char* single_log, int size, bool is_quit = false);   //将日志写入buffer输出缓冲区中
        void Start();    //开始线程
        void Stop();     //停止线程

    private:
        void Worker();  //线程函数 将buffer的数据写入日志文件

    private:
        std::string file_name_;
        const int timeout_;
        bool is_running_;

        std::shared_ptr<Buffer> current_buffer_;
        std::shared_ptr<Buffer> next_buffer_;
        std::vector<std::shared_ptr<Buffer>> buffers_;

        thread::Thread thread_;
        mutable locker::MutexLock mutex_;
        locker::ConditionVariable condition_;
        utility::CountDownLatch count_down_latch_;
    };

}  // namespace log

#endif  // LOG_ASYNC_LOGGING_H_
