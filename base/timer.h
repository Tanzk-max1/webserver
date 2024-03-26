//
// Created by 15525 on 2024/3/26.
//

#ifndef WEBSER_TIMER_H
#define WEBSER_TIMER_H

#include <unistd.h>

#include <memory>

//类的前置声明
namespace http {
    class HttpConnection;
}  // namespace http

namespace timer {
    //定时器类
    class Timer {
    public:
        Timer(std::shared_ptr<http::HttpConnection> http, int timeout);
        //拷贝构造
        Timer(Timer& timer);
        ~Timer();

        //更新到期时间 = 当前时间 + 超时时间
        void Update(int timeout);
        //是否到期
        bool is_expired();
        //释放http
        void Release();

        //得到到期时间
        int expire_time() const {
            return expire_time_;
        }

        //http是否已经删除
        bool is_deleted() const {
            return is_deleted_;
        }

    private:
        std::shared_ptr<http::HttpConnection> http_connection_;
        int expire_time_;
        bool is_deleted_;
    };

}  // namespace timer


#endif//WEBSER_TIMER_H
