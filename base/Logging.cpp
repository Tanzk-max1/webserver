//
// Created by 15525 on 2024/3/17.
//
#include "Logging.h"
#include "CurrentThread.h"
#include "Thread.h"
#include "AsyncLogging.h"
#include <assert.h>
#include <iostream>
#include <time.h>
#include <sys/time.h>


static pthread_once_t once_control_ = PTHREAD_ONCE_INIT;
static AsyncLogging *AsyncLogger_;

Logger::Logger(const char *fileName, int line)
    : impl_(fileName , line){}

Logger::~Logger() {
    impl_.stream_ << " -- " << impl_.basename_ << ':' << impl_.line_ << "\n";
    const LogStream::Buffer & buf(stream().buffer());
    output(buf.data(),buf.length());
}
