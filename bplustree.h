//
//  bplustree.h
//  bplustree
//
//  Created by Xw on 2017/5/31.
//  Copyright © 2017年 Xw. All rights reserved.
//

#ifndef _BPLUSTREE_H_
#define _BPLUSTREE_H_ 1

#include <vector>
#include <string>

//模板类TreeNode，用于存放B+树的结点数据以及进行相关的操作
//使用模板类保证直接适配int，float, string三种类型数据
//减少代码规模
template <typename T>
class TreeNode{
public:
	//该结点内key数量
    unsigned int num;
	//指向父节点指针
    TreeNode* parent;
	//存放key容器
    vector <T> keys;
	//指向子结点的指针容器
    vector <TreeNode*> childs;
    vector <int> vals;
	//指向下一个叶结点的指针
    TreeNode* nextLeafNode;
    //此结点是否是叶结点的标志
    bool isLeaf;
	//此树的度
    int degree;

public:
	//构造函数
	//输入：树的度，是否为叶结点
    //功能：初始化一个新的结点，若Leaf=true则创建一个叶结点，否则创建一个枝干结点
    TreeNode(int in_degree, bool Leaf = false);
	//析构函数
    ~TreeNode();
    //功能：判断是否为根结点
    bool isRoot();
	//输入：key值，(index)
    //输出：bool
    //功能：判断结点内是否存在某key值，并通过index引用返回该值index
    //并通过函数返回bool值说明是否找到
    bool findKey(T key, unsigned int &index);
    //输出：TreeNode指针
    //功能：将一个结点分裂成两个结点(与B+树的分裂有关)
	//新结点为本结点的下一个结点
    //同时功过key引用返回去到上层的key值
    TreeNode* splitNode(T &key);
	//输入：key值
    //输出：添加的position
    //功能：在枝干结点中添加key值，并返回添加的位置
    unsigned int addKey(T &key);
	//输入：key值
    //输出：添加的position
    //功能：在叶结点中添加key值，并返回添加的位置
    unsigned int addKey(T &key, int val);
	//输入：key值对应的index
    //输出：bool
    //功能：在结点中删除index对应的key值
    bool deleteKeyByIndex(unsigned int index);
};

//模板类BPlusTree，用于对整个B+树的操作
//由此模块操作TreeNode模块，并且由此模块与buffer_manager模块进行交互
//完成文件的读写操作
template <typename T>
class BPlusTree
{
private:
	//重命名TreeNode指针，方便后边进行操作区分
    typedef TreeNode<T>* Tree;
	//特殊结构，用于临时存储查找的key值及所处的位置，方便进行操作
    struct searchNodeParse {
        Tree pNode; //包含对应key的结点指针
        unsigned int index; //key在结点中的index
        bool ifFound; //是否找到该key
    };
private:
	//文件名，用于文件读写操作
    std::string file_name;
	//根结点指针
    Tree root;
	//叶结点的左部头指针
    Tree leafHead;
	//此树key的数量
    unsigned int key_num;
	//此树的level数量
    unsigned int level;
	//此树的node数量
    unsigned int node_num;
    //fileNode* file; // the filenode of this tree
	//每个key值的size，对于一颗树来说，所有key的size应是相同的
    int key_size;
	//树的度
    int degree;

public:
	//构造函数
	//用于构造一颗新的树，确定m_name,key的size，树的度
	//同时调用其他函数为本树分配内存
    BPlusTree(std::string m_name, int key_size, int degree);
	//析构函数
	//释放相应的内存
    ~BPlusTree();

	//
    int searchVal(T& key);
	//输入：key值及其value
    //输出：bool
    //功能：在树中插入一个key值
	//返回是否插入成功
    bool insertKey(T &key, int val);
	//输入：key值
    //输出：bool
    //功能：在树中删除一个key值
	//返回是否删除成功
    bool deleteKey(T &key);
	//输入：树的根结点
    //功能：删除整棵树并释放内存空间，主要用在析构函数中
    void dropTree(Tree node);

	//从磁盘读取所有数据
    void readFromDiskAll();
	//将新数据写入磁盘
    void writtenbackToDiskAll();
	//在磁盘中读取某一块的数据
    void readFromDisk(blockNode* btmp);

private:
	//初始化B+树，并分配内存空间
    void initTree();
	//用于插入某key后结点满情况，对整棵树进行调整
    bool adjustAfterinsert(Tree pNode);
	//用于删除某key后可能出现结点空情况，对整棵树进行调整
    bool adjustAfterDelete(Tree pNode);
	//用于查找某key值所处的叶结点位置
    void findToLeaf(Tree pNode, T key, searchNodeParse &snp);

private:
	//用于对buffer_manager模块的操作
	BufferManager buffer;
};


#endif
