//
//  buffer_manager.cc
//  buffer_manager
//
//  Created by Hys on 2017/5/21.
//  Copyright © 2017年 Hys. All rights reserved.
//
#include "buffer_manager.h"

// Page类的实现
Page::Page() {
    initialize();
}
// 初始化
void Page::initialize() {
    file_name_ = "";
    block_id_ = -1;
    pin_count_ = -1;
    dirty_ = false;
    ref_ = false;
    avaliable_ = true;
    for (int i = 0;i < PAGESIZE;i++) 
        buffer_[i] = '\0';
}

// 下面是一些存取控制函数，较为简单就不赘述了
inline void Page::setFileName(std::string file_name) {
    file_name_ = file_name;
}

inline std::string Page::getFileName() {
    return file_name_;
}

inline void Page::setBlockId(int block_id) {
    block_id_ = block_id;
}

inline int Page::getBlockId() {
    return block_id_;
}

inline void Page::setPinCount(int pin_count) {
    pin_count_ = pin_count;
}

inline int Page::getPinCount() {
    return pin_count_;
}

inline void Page::setDirty(bool dirty) {
    dirty_ = dirty;
}

inline bool Page::getDirty() {
    return dirty_;
}

inline void Page::setRef(bool ref) {
    ref_ = ref;
}

inline bool Page::getRef() {
    return ref_;
}

inline void Page::setAvaliable(bool avaliable) {
    avaliable_ = avaliable;
}

inline bool Page::getAvaliable() {
    return avaliable_;
}

inline char* Page::getBuffer() {
    return buffer_;
}

// BufferManager类的实现
// 构造函数均调用实际初始化函数完成初始化
BufferManager::BufferManager() {
    initialize(MAXFRAMESIZE);
}

BufferManager::BufferManager(int frame_size) {
    initialize(frame_size);
}

// 析构函数非常重要。在程序结束时需要将缓冲池里的所有页写回磁盘。
BufferManager::~BufferManager() {
    for (int i = 0;i < frame_size_;i++) {
        std::string file_name;
        int block_id;
        file_name = Frames[i].getFileName();
        block_id = Frames[i].getBlockId();
        flushPage(i , file_name , block_id);
    }
}

// 实际初始化函数
void BufferManager::initialize(int frame_size) {
    Frames = new Page[frame_size];//在堆上分配内存
    frame_size_ = frame_size;
    current_position_ = 0;
}

// 下面几个函数较为简单，也不赘述了
char* BufferManager::getPage(std::string file_name , int block_id) {
    int page_id = getPageId(file_name , block_id);
    if (page_id == -1) {
        page_id = getEmptyPageId();
        loadDiskBlock(page_id , file_name , block_id);
    }
    Frames[page_id].setRef(true);
    return Frames[page_id].getBuffer();
}

void BufferManager::modifyPage(int page_id) {
    Frames[page_id].setDirty(true);
}

void BufferManager::pinPage(int page_id) {
    int pin_count = Frames[page_id].getPinCount();
    Frames[page_id].setPinCount(pin_count + 1);
}

int BufferManager::unpinPage(int page_id) {
    int pin_count = Frames[page_id].getPinCount();
    if (pin_count <= 0) 
        return -1;
    else
        Frames[page_id].setPinCount(pin_count - 1);
    return 0;
}

// 核心函数之一。内存和磁盘交互的接口。
int BufferManager::loadDiskBlock(int page_id , std::string file_name , int block_id) {
    // 初始化一个页
    Frames[page_id].initialize();
    // 打开磁盘文件
    FILE* f = fopen(file_name.c_str() , "r");
    // 打开失败返回-1
    if (f == NULL)
        return -1;
    // 将文件指针定位到对应位置
    fseek(f , PAGESIZE * block_id , SEEK_SET);
    // 获取页的句柄
    char* buffer = Frames[page_id].getBuffer();
    // 读取对应磁盘块到内存页
    fread(buffer , PAGESIZE , 1 , f);
    // 关闭文件
    fclose(f);
    // 对新载入的页进行相应设置
    Frames[page_id].setFileName(file_name);
    Frames[page_id].setBlockId(block_id);
    Frames[page_id].setPinCount(1);
    Frames[page_id].setDirty(false);
    Frames[page_id].setRef(true);
    Frames[page_id].setAvaliable(false);
    return 0;
}

// 核心函数之一。内存和磁盘交互的接口。
int BufferManager::flushPage(int page_id , std::string file_name , int block_id) {
    // 打开文件
    FILE* f = fopen(file_name.c_str() , "r+");
    // 其实这里有写多余，因为打开一个文件读总是能成功。
    if (f == NULL)
        return -1; 
    // 将文件指针定位到对应位置
    fseek(f , PAGESIZE * block_id , SEEK_SET);
    // 获取页的句柄
    char* buffer = Frames[page_id].getBuffer();
    // 将内存页的内容写入磁盘块
    fwrite(buffer , PAGESIZE , 1 , f);
    // 关闭文件
    fclose(f);
    return 0;
}

// 简单遍历获取页号
int BufferManager::getPageId(std::string file_name , int block_id) {
    for (int i = 0;i < frame_size_;i++) {
        std::string tmp_file_name = Frames[i].getFileName();
        int tmp_block_id = Frames[i].getBlockId();
        if (tmp_file_name == file_name && tmp_block_id == block_id)
            return i;
    }
    return -1;
}

// 寻找一个闲置的页
int BufferManager::getEmptyPageId() {
    // 先简单的遍历一遍，如果有闲置的直接返回其页号
    for (int i = 0;i < frame_size_;i++) {
        if (Frames[i].getAvaliable() == true)
            return i;
    }
    // 如果所有页都已经被使用，那么需要找到一个页，将其删除掉。
    // 这里需要使用一些策略来选择哪一个页应该被删除。
    // 本程序中采用时钟替换策略。
    while (1) {
        // 如果页的ref为true，将其设为false
        if (Frames[current_position_].getRef() == true) {
            Frames[current_position_].setRef(false);
        }
        // 否则，如果页对应的pin_count为0，即页没有被锁住，那么这一页就
        // 会被删除。
        else if (Frames[current_position_].getPinCount() == 0) {
            // 如果这一页被改动过，需要将其写回磁盘，然后删除
            if (Frames[current_position_].getDirty() == true) {
                std::string file_name = Frames[current_position_].getFileName();
                int block_id = Frames[current_position_].getBlockId();
                flushPage(current_position_ , file_name , block_id);
            }
            // 删除页
            Frames[current_position_].initialize();
            // 返回页号
            return current_position_;
        }
        // 时钟指针顺时针转动
        current_position_ = (current_position_ + 1) % frame_size_;
    }
}