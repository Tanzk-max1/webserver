//
// Created by 15525 on 2024/3/17.
//

#ifndef WEBSER_NONCOPYABLE_H
#define WEBSER_NONCOPYABLE_H

//#endif//WEBSER_NONCOPYABLE_H
//
//#pragma once
//
//class noncopyable {
//protected:
//    noncopyable() {}
//    ~noncopyable() {}
//
//private:
//    noncopyable(const noncopyable&);
//    const noncopyable& operator=(const noncopyable&);
//};

namespace utility {
    // 删除拷贝构造和拷贝赋值 子类继承此父类时 子类如果没有自己定义就会默认调用父类的
    class NonCopyAble {
    public:
        NonCopyAble(const NonCopyAble&) = delete;
        NonCopyAble& operator=(const NonCopyAble&) = delete;

        NonCopyAble(NonCopyAble&&) = default;
        NonCopyAble& operator=(NonCopyAble&&) = default;

    protected:
        NonCopyAble() = default;
        ~NonCopyAble() = default;
    };

}  // namespace utility

#endif  // UTILITY_NONCOPYABLE_H_