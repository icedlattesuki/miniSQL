//
//  buffer_manager.h
//  buffer_manager
//
//  Created by Hys on 2017/5/21.
//  Copyright © 2017年 Hys. All rights reserved.
//
#ifndef _BUFFER_MANAGER_H_
#define _BUFFER_MANAGER_H_ 1

#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
#include "const.h"

// Page类。磁盘文件中的每一块对应内存中的一个页（page)
class Page {
    public:
        // 构造函数和一些存取控制函数。可忽略。
        Page();
        void initialize();
        void setFileName(std::string file_name);
        std::string getFileName();
        void setBlockId(int block_id);
        int getBlockId();
        void setPinCount(int pin_count);
        int getPinCount();
        void setDirty(bool dirty);
        bool getDirty();
        void setRef(bool ref);
        bool getRef();
        void setAvaliable(bool avaliable);
        bool getAvaliable();
        char* getBuffer();
    private:
        char buffer_[PAGESIZE];//每一页都是一个大小为PAGESIZE字节的数组
        std::string file_name_;//页所对应的文件名
        int block_id_;//页在所在文件中的块号(磁盘中通常叫块)
        int pin_count_;//记录被钉住的次数。被钉住的意思就是不可以被替换
        bool dirty_;//dirty记录页是否被修改
        bool ref_;//ref变量用于时钟替换策略
        bool avaliable_;//avaliable标示页是否可以被使用(即将磁盘块load进该页)
};

// BufferManager类。对外提供操作缓冲区的接口。
class BufferManager {
    public: 
        //  构造函数
        BufferManager();
        BufferManager(int frame_size);
        // 析构函数
        ~BufferManager();
        // 通过页号得到页的句柄(一个页的头地址)
        char* getPage(std::string file_name , int block_id);
        // 标记page_id所对应的页已经被修改
        void modifyPage(int page_id);
        // 钉住一个页
        void pinPage(int page_id);
        // 解除一个页的钉住状态(需要注意的是一个页可能被多次钉住，该函数只能解除一次)
        // 如果对应页的pin_count_为0，则返回-1
        int unpinPage(int page_id);
        // 将对应内存页写入对应文件的对应块。这里的返回值为int，但感觉其实没什么用，可以设为void
        int flushPage(int page_id , std::string file_name , int block_id);
        // 获取对应文件的对应块在内存中的页号，没有找到返回-1
        int getPageId(std::string file_name , int block_id);
    private:
        Page* Frames;//缓冲池，实际上就是一个元素为Page的数组，实际内存空间将分配在堆上
        int frame_size_;//记录总页数
        int current_position_;//时钟替换策略需要用到的变量
        void initialize(int frame_size);//实际初始化函数
        // 获取一个闲置的页的页号(内部封装了时钟替换策略，但使用者不需要知道这些)
        int getEmptyPageId();
        // 讲对应文件的对应块载入对应内存页，对于文件不存在返回-1，否则返回0
        int loadDiskBlock(int page_id , std::string file_name , int block_id);
};

#endif

// 如何使用?
// 首先实例化一个BufferManager类，然后通过getPage接口得到对应文件的对应块
// 在内存中的句柄，得到句柄之后即可读取或修改页的内容。之后通过getPageId接口
// 获取块在内存中的页号。需要注意的是，如果修改了对应页的内容，需要调用
// modifyPage接口来标记该页已经被修改，否则可能修改会丢失。
// 另外，如果当前操作需要持续使用某一页，则需要通过pinPage接口将该页钉住，防止
// 被替换。如果不需要再使用该页，通过unpinPage接口将该页解除。
// 通过flushPage接口可以将内存中的一页写到文件中的一块。