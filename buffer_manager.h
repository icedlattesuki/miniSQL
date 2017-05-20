#ifndef _BUFFER_MANAGER_H_
#define _BUFFER_MANAGER_H_ 1

// 每一页的大小为4KB
#define PAGESIZE 4096
// 最大页数为100
#define MAXFRAMESIZE 100
#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

// Page类。磁盘文件中的每一块对应内存中的一个页（page)
class Page {
    public:
        // 构造函数和一些存取控制函数。可忽略。
        Page();
        void initialize();
        void setFileName(string file_name);
        string getFileName();
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
        string file_name_;//页所对应的文件名
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
        char* getPage(int page_id);
        // 标记page_id所对应的页已经被修改
        void modifyPage(int page_id);
        // 钉住一个页
        void pinPage(int page_id);
        // 解除一个页的钉住状态(需要注意的是一个页可能被多次钉住，该函数只能解除一次)
        // 如果对应页的pin_count_为0，则返回-1
        int unpinPage(int page_id);
        // 讲对应文件的对应块载入对应内存页，对于文件不存在返回-1，否则返回0
        int loadDiskBlock(int page_id , string file_name , int block_id);
        // 将对应内存页写入对应文件的对应块。这里的返回值为int，但感觉其实没什么用，可以设为void
        int flushPage(int page_id , string file_name , int block_id);
        // 获取对应文件的对应块在内存中的页号，没有找到返回-1
        int getPageId(string file_name , int block_id);
        // 获取一个闲置的页的页号(内部封装了时钟替换策略，但使用者不需要知道这些)
        int getEmptyPageId();
    private:
        Page* Frames;//缓冲池，实际上就是一个元素为Page的数组，实际内存空间将分配在堆上
        int frame_size_;//记录总页数
        int current_position_;//时钟替换策略需要用到的变量
        void initialize(int frame_size);//实际初始化函数
};

#endif

// 如何使用?
// 首先需要实力化一个BufferManager类。
// 接着如果想要访问某一个文件的某一块，那么需要先通过getPageId接口来获取
// 其在缓冲池中的页数。如果返回结果为-1，说明其不在缓冲池中，因此需要使用
// getEmptyPageId接口来获取一个可以使用的页，然后通过loadDiskBlock接口
// 来将块载入缓冲池。如果返回结果不为-1，则说明对应的块已经在缓冲池里了，不
// 需要再手动将其载入内存。得到块所对应的page_id后，通过getPage接口来获取
// 页的句柄。得到句柄后即可读取和修改对应页的内容。需要注意的是，如果修改了对应
// 页的内容，需要调用modifyPage接口来标记该块已经被修改，否则可能修改会丢失。
// 另外，如果当前操作需要持续使用某一页，则需要通过pinPage接口将该页钉住，防止
// 被替换。如果不需要再使用该页，通过unpinPage接口将该页解除。