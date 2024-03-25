//
// Created by 15525 on 2024/3/20.
//
//内存池部分参考了       https://github.com/cacay/MemoryPool/tree/master/C-11

#include "MemoryPool.h"
#include <assert.h>
namespace memoryPool{
MemoryPool::MemoryPool() {}

MemoryPool::~MemoryPool() {
    Slot* cur = currentBlock_;
    while (cur){
        Slot* next = cur->next;
        operator delete (reinterpret_cast<void *>(cur));
        cur = next;
    }
    }

    void MemoryPool::init(int size) {
        assert(size > 0);  // 断言，确保 size 大于 0

        slotSize_ = size;  // 将 size 赋值给成员变量 slotSize_
        currentBolck_ = NULL;  // 将指针 currentBolck_ 置为 NULL
        currentSlot_ = NULL;  // 将指针 currentSlot_ 置为 NULL
        lastSlot_ = NULL;  // 将指针 lastSlot_ 置为 NULL
        freeSlot_ = NULL;  // 将指针 freeSlot_ 置为 NULL
    }
    //assert(size > 0): assert 是一个宏，用于在程序中进行断言检查。在这里，它确保传入的 size 大于 0，如果不满足条件，则会触发断言错误。
    //slotSize_ = size: 将参数 size 的值赋给成员变量 slotSize_。这里的 slotSize_ 是一个类成员变量，用于存储槽的大小。
    //currentBolck_ = NULL: 将指针 currentBolck_ 置为 NULL，表示当前块的指针为空。
    //currentSlot_ = NULL: 将指针 currentSlot_ 置为 NULL，表示当前槽的指针为空。
    //lastSlot_ = NULL: 将指针 lastSlot_ 置为 NULL，表示最后一个槽的指针为空。
    //freeSlot_ = NULL: 将指针 freeSlot_ 置为 NULL，表示空闲槽的指针为空。



    //计算对其所需要的空间
    // 计算指针 p 对齐后的偏移量
    // 参数 p: 指向某个内存位置的字符型指针
    // 参数 align: 对齐要求，以字节为单位
    // 返回值: 对齐后的偏移量

    inline size_t MemoryPool::padPointer(char* p, size_t align) {
        // 将指针 p 转换为 size_t 类型，以获取其地址值
        size_t result = reinterpret_cast<size_t>(p);

        // 计算对齐后的偏移量，并返回
        // 对齐后的偏移量等于对齐要求减去地址值 result 对齐要求的余数
        return ((align - result) % align);
    }

    Slot* MemoryPool::allocateBlock() {
        char* newBlock = reinterpret_cast<char*>(operator new(BlockSize));  // 在堆上分配一个新的块，大小为 BlockSize，并将其转换为 char* 类型指针

        char* body = newBlock + sizeof(Slot*);  // 计算块的起始位置，即 newBlock 指针加上 Slot* 的大小
        // 计算为了对齐需要空出多少位置
        // size_t bodyPadding = padPointer(body, sizeof(slotSize_));
        size_t bodyPadding = padPointer(body, static_cast<size_t>(slotSize_));  // 计算 body 的内存对齐填充量
        // 注意：多个线程（eventLoop共用一个MemoryPool）
        Slot* useSlot;  // 用于存储被分配的槽的指针

        {
            MutexLockGuard lock(mutex_other_);  // 使用互斥锁保护共享资源

            reinterpret_cast<Slot*>(newBlock)->next = currentBlock_;  // 将 newBlock 转换为 Slot* 类型，并设置其 next 指针指向当前块

            currentBlock_ = reinterpret_cast<Slot*>(newBlock);  // 将当前块指针指向 newBlock

            currentSlot_ = reinterpret_cast<Slot*>(body + bodyPadding);  // 将当前槽指针指向 body + bodyPadding 的位置

            lastSlot_ = reinterpret_cast<Slot*>(newBlock + BlockSize - slotSize_ + 1);  // 将最后一个槽指针指向 newBlock + BlockSize - slotSize_ + 1 的位置

            useSlot = currentSlot_;  // 将 useSlot 设置为当前槽

            currentSlot_ += (slotSize_ >> 3);  // 将当前槽指针向后移动一定偏移量 (slotSize_ >> 3)
        }

        return useSlot;  // 返回被分配的槽的指针
    }

    // 在内存池中分配一个内存块，并返回指向该内存块的指针
    Slot* MemoryPool::nofree_solve() {
        if(currentSlot_ >= lastSlot_)
            return allocateBlock(); // 如果当前内存块不足，则分配一个新的内存块
        Slot* useSlot;
        {
            locker::LockGuard lock(mutex_other_);
            useSlot = currentSlot_;
            currentSlot_ += (slotSize_ >> 3); // 将当前指针向后移动，指向下一个可用的内存块
        }
        return useSlot;
    }

    // 分配一个指定大小的内存块
    Slot* MemoryPool::allocate() {
        if(freeSlot_) {
            {
                locker::LockGuard lock(mutex_freeSlot_);
                if(freeSlot_) {
                    Slot* result = freeSlot_;
                    freeSlot_ = freeSlot_->next; // 从空闲内存块链表中取出一个内存块
                    return result;
                }
            }
        }
        return nofree_solve(); // 当没有空闲内存块时，调用nofree_solve()来分配内存块
    }

    // 释放一个内存块，并将其加入空闲内存块链表
    inline void MemoryPool::deAllocate(Slot* p) {
        if(p) {
            locker::LockGuard lock(mutex_freeSlot_);
            p->next = freeSlot_; // 将要释放的内存块加入空闲内存块链表的头部
            freeSlot_ = p;
        }
    }

    // 获取指定ID的内存池对象
    MemoryPool& get_MemoryPool(int id) {
        static MemoryPool memorypool_[64]; // 内存池对象数组
        return memorypool_[id]; // 返回指定ID的内存池对象引用
    }

    // 初始化内存池数组，每个内存池对象管理不同大小的内存块链表
    void init_MemoryPool() {
        for(int i = 0; i < 64; ++i) {
            get_MemoryPool(i).init((i + 1) << 3); // 初始化每个内存池对象，设置内存块大小
        }
    }

    // 分配指定大小的内存，如果超过512字节则直接使用new操作符
    void* use_Memory(size_t size) {
        if(!size)
            return nullptr;
        if(size > 512)
            return operator new(size); // 超过512字节，直接使用new操作符分配内存
        // 根据内存块大小选择对应的内存池，进行内存分配
        return reinterpret_cast<void *>(get_MemoryPool(((size + 7) >> 3) - 1).allocate());
    }

    // 释放内存，如果超过512字节则直接使用delete操作符
    void free_Memory(size_t size, void* p) {
        if(!p)  return;
        if(size > 512) {
            operator delete (p); // 超过512字节，直接使用delete操作符释放内存
            return;
        }
        // 根据内存块大小选择对应的内存池，进行内存释放
        get_MemoryPool(((size + 7) >> 3) - 1).deAllocate(reinterpret_cast<Slot *>(p));
    }
}

