//
// Created by 15525 on 2024/3/20.
//

#ifndef WEBSER_MEMORYPOOL_H
#define WEBSER_MEMORYPOOL_H

#endif//WEBSER_MEMORYPOOL_H

#define BlockSize 4096

struct Slot {
    Slot* next;

};

class MemoryPool {
public:
    MemoryPool();
    ~MemoryPool();

    void init(int size);

    //分配或回收一个元素的内存空间
};