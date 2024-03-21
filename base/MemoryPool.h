//
// Created by 15525 on 2024/3/20.
//

#ifndef WEBSER_MEMORYPOOL_H
#define WEBSER_MEMORYPOOL_H

#endif//WEBSER_MEMORYPOOL_H
#include "MutexLock.h"

#define BlockSize 4096

struct Slot {
    Slot* next;

};

class MemoryPool {
public:
    MemoryPool();
    ~MemoryPool();

    void init(int size);

    Slot *allocate();
    void deAllocate(Slot *p);

private:
    int slotSize_;// 每个槽所占字节数

    Slot *currentBlock_;// 内存块链表的头指针
    Slot *currentSlot_; // 元素链表的头指针
    Slot *lastSlot_;    // 可存放元素的最后指针
    Slot *freeSlot_;    // 元素构造后释放掉的内存链表头指针

    MutexLock mutex_freeSlot_;
    MutexLock mutex_other_;
    size_t padPointer(char *p, size_t align);
    Slot *allocateBlock();//申请内存快放进内存池
    Slot *nofree_slove();

    //分配或回收一个元素的内存空间
};

void init_MemoryPool();
void* use_Memory(size_t size);
void free_Memory(size_t size,void* p);
MemoryPool& get_MemoryPool(int id);

