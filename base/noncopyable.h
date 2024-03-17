//
// Created by 15525 on 2024/3/17.
//

#ifndef WEBSER_NONCOPYABLE_H
#define WEBSER_NONCOPYABLE_H

#endif//WEBSER_NONCOPYABLE_H

#pragma once

class noncopyable {
protected:
    noncopyable() {}
    ~noncopyable() {}

private:
    noncopyable(const noncopyable&);
    const noncopyable& operator=(const noncopyable&);
};
