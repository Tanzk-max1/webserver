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



LogStream& LogStream::operator<<(short v) {
    *this << static_cast<int>(v);
    return *this;
}