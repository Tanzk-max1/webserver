//
// Created by 15525 on 2024/3/17.
//

//#ifndef WEBSER_LOGGING_H
//#define WEBSER_LOGGING_H
//
//
//#pragma once
//#include <pthread.h>
//#include <stdio.h>
//#include <string>
//#include <string.h>
//#include "LogStream.h"
//
//namespace lpg{
//    class Logging{
//    public:
//        Logging(const char * file_name,int line,ine level);
//        ~Logging();
//
//        static std::string log_file_name() {
//            return log_file_name_;
//        }
//
//        static void set_log_file_name(std::string log_file_name) {
//            log_file_name_ = log_file_name;
//        }
//
//        static bool open_log() {
//            return open_log_;
//        }
//
//        static void set_open_log(bool open_log) {
//            open_log_ = open_log;
//        }
//
//        static void set_log_to_stderr(bool log_to_stderr) {
//            log_to_stderr_ = log_to_stderr;
//        }
//
//        static void set_color_log_to_stderr(bool color_log_to_stderr) {
//            color_log_to_stderr_ = color_log_to_stderr;
//        }
//
//        static void set_min_log_level(int min_log_level) {
//            min_log_level_ = min_log_level;
//        }
//
//        LogStream& stream() {
//            return impl_.stream_;
//        }
//
//    private:
//        class Impl {
//        public:
//            Impl(const char* file_name, int line, int level);
//            void FormatLevel();
//            void FormatTime();
//
//            LogStream stream_;
//            std::string file_name_;
//            int line_;
//            int level_;
//            std::string level_str_;
//            std::string log_color_;
//            char time_str_[26];
//            bool is_fatal_;
//        };
//
//    private:
//        static std::string log_file_name_;
//        static bool open_log_;
//        static bool log_to_stderr_;
//        static bool color_log_to_stderr_;
//        static int min_log_level_;
//
//        Impl impl_;
//    };
//}
//

#ifndef LOG_LOGGING_H_
#define LOG_LOGGING_H_

#define OPEN_LOGGING

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <string>

#include "LogStream.h"

namespace log {
    class Logging {
    public:
        Logging(const char* file_name, int line, int level);
        ~Logging();

        static std::string log_file_name() {
            return log_file_name_;
        }

        static void set_log_file_name(std::string log_file_name) {
            log_file_name_ = log_file_name;
        }

        static bool open_log() {
            return open_log_;
        }

        static void set_open_log(bool open_log) {
            open_log_ = open_log;
        }

        static void set_log_to_stderr(bool log_to_stderr) {
            log_to_stderr_ = log_to_stderr;
        }

        static void set_color_log_to_stderr(bool color_log_to_stderr) {
            color_log_to_stderr_ = color_log_to_stderr;
        }

        static void set_min_log_level(int min_log_level) {
            min_log_level_ = min_log_level;
        }

        LogStream& stream() {
            return impl_.stream_;
        }

    private:
        class Impl {
        public:
            Impl(const char* file_name, int line, int level);
            void FormatLevel();
            void FormatTime();

            LogStream stream_;
            std::string file_name_;
            int line_;
            int level_;
            std::string level_str_;
            std::string log_color_;
            char time_str_[26];
            bool is_fatal_;
        };

    private:
        static std::string log_file_name_;
        static bool open_log_;
        static bool log_to_stderr_;
        static bool color_log_to_stderr_;
        static int min_log_level_;

        Impl impl_;
    };

}  // namespace log

enum LogLevel {
    DEBUG = 0,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

//宏定义
#ifdef OPEN_LOGGING
#define LOG(level)  log::Logging(__FILE__, __LINE__, level).stream()
#define LOG_DEBUG   log::Logging(__FILE__, __LINE__, DEBUG).stream()
#define LOG_INFO    log::Logging(__FILE__, __LINE__, INFO).stream()
#define LOG_WARNING log::Logging(__FILE__, __LINE__, WARNING).stream()
#define LOG_ERROR   log::Logging(__FILE__, __LINE__, ERROR).stream()
#define LOG_FATAL   log::Logging(__FILE__, __LINE__, FATAL).stream()
#else
#define LOG(level) log::LogStream()
#endif


#endif//WEBSER_LOGGING_H

//class Logger {
//public:
//    Logger(const char* filename,int line);
//    ~Logger();
//    LogStream& stream() {return impl_.stream_; }
//
//    static void setLogFileName(std::string fileName) { logFileName_ = fileName;}
//    static std::string getLogFileName() { return logFileName_;}
//private:
//    //Logger的内部实现类Impl：负责把⽇志头信息写⼊logstream
//    //包含：时间戳，LogStream数据流，源⽂件⾏号，源⽂件名字
//    //TODO:加⼊⽇志级别
//
//    class Impl{
//    public:
//        Impl( const char * fileName , int line);
//        void formaTime();
//
//        LogStream stream_;
//        int line_;
//        std::string basename_;
//
//    };
//    Impl impl_;
//    static std::string logFileName_;
//    // C/C++提供了三个宏来定位程序运⾏时的错误
//    // __FUNCTION__:返回当前所在的函数名
//    // __FILE__:返回当前的⽂件名
//    // __LINE__:当前执⾏⾏所在⾏的⾏号
//
//#define LOG Logger(__FILE__, __LINE__).stream()
//
//};