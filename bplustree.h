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
#include <map>
#include <iostream>
#include <sstream>
#include "basic.h"
#include "const.h"
#include "exception.h"
#include "buffer_manager.h"
#include "template_function.h"

extern BufferManager buffer_manager;

//模板类TreeNode，用于存放B+树的结点数据以及进行相关的操作
//使用模板类保证直接适配int，float, string三种类型数据
//减少代码规模
template <typename T>
class TreeNode {
public:
	//该结点内key数量
    unsigned int num;
	//指向父节点指针
    TreeNode* parent;
	//存放key容器
    std::vector <T> keys;
	//指向子结点的指针容器
    std::vector <TreeNode*> childs;
    std::vector <int> vals;
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
    //返回下一个相邻叶结点指针
    TreeNode* nextLeaf();
    //输入:起始index,终止key，返回结果vector容器
    //输出:到达终止key返回true，否则返回flase
    //功能：返回一定范围的value容器
    bool findRange(unsigned int index, T& key, std::vector<int>& vals);
    bool findRange2(unsigned int index, std::vector<int>& vals);
    
    void printl();
};

//模板类BPlusTree，用于对整个B+树的操作
//由此模块操作TreeNode模块，并且由此模块与buffer_manager模块进行交互
//完成文件的读写操作
template <typename T>
class BPlusTree {
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

	//根据key值返回对应的Value
    int searchVal(T &key);
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
    //输入：key1，key2，返回vals的容器
    //功能：返回范围搜索结果，将value放入vals容器中
    void searchRange(T &key1, T &key2, std::vector<int>& vals, int flag);

	//从磁盘读取所有数据
    void readFromDiskAll();
	//将新数据写入磁盘
    void writtenbackToDiskAll();
	//在磁盘中读取某一块的数据
    void readFromDisk(char *p, char* end);
    
    void printleaf();

private:
	//初始化B+树，并分配内存空间
    void initTree();
	//用于插入某key后结点满情况，对整棵树进行调整
    bool adjustAfterinsert(Tree pNode);
	//用于删除某key后可能出现结点空情况，对整棵树进行调整
    bool adjustAfterDelete(Tree pNode);
	//用于查找某key值所处的叶结点位置
    void findToLeaf(Tree pNode, T key, searchNodeParse &snp);
    //获取文件大小
    void getFile(std::string file_path);
    int getBlockNum(std::string table_name);
};

template <class T>
TreeNode<T>::TreeNode(int in_degree, bool Leaf):
	num(0),
	parent(NULL),
	nextLeafNode(NULL),
	isLeaf(Leaf),
	degree(in_degree)
{
    for (unsigned i = 0; i < degree+1; i++) {
        childs.push_back(NULL);
        keys.push_back(T());
        vals.push_back(int());
    }

    childs.push_back(NULL);
}

//析构函数使用默认析构函数即可
template <class T>
TreeNode<T>::~TreeNode()
{

}

//功能：判断是否为根结点
template <class T>
bool TreeNode<T>::isRoot()
{
    if (parent != NULL)
		return false;
    else
		return true;
}

//输入：key值，(index)
//输出：bool
//功能：判断结点内是否存在某key值，并通过index引用返回该值index
//如不存在，则返回最接近的index
//并通过函数返回bool值说明是否找到
template <class T>
bool TreeNode<T>::findKey(T key, unsigned int &index)
{
    if (num == 0) { //结点内key数量为0
        index = 0;
        return false;
    } else {
        //判断key值是否超过本结点内最大值(key不在本结点内)
        if (keys[num-1] < key) {
            index = num;
            return false;
		//判断key值是否小于本结点内最小值(key不在本结点内)
        } else if (keys[0] > key) {
            index = 0;
            return false;
        } else if (num <= 20) {
		//结点内key数量较少时直接线性遍历搜索即可
            for (unsigned int i = 0; i < num; i++) {
                if (keys[i] == key) {
                    index = i;
                    return true;
                } else if (keys[i] < key)
                    continue;
                else if(keys[i] > key) {
                    index = i;
                    return false;
                }
            }
        } else if(num > 20) {
		//结点内key数量过多时采用二分搜索
            unsigned int left = 0, right = num-1, pos = 0;

            while (right > left+1) {
                pos = (right + left) / 2;
                if (keys[pos] == key) {
                    index = pos;
                    return true;
                } else if (keys[pos] < key) {
                    left = pos;
                } else if (keys[pos] > key) {
                    right = pos;
                }
            }

            if (keys[left] >= key) {
                index = left;
                return (keys[left] == key);
            } else if (keys[right] >= key) {
                index = right;
                return (keys[right] == key);
            } else if(keys[right] < key) {
                index = right ++;
                return false;
            }
        }//二分搜索结束
    }

    return false;
}

//输出：TreeNode指针
//功能：将一个结点分裂成两个结点(与B+树的分裂有关)
//新结点为本结点的下一个结点
//同时功过key引用返回去到上层的key值
template <class T>
TreeNode<T>* TreeNode<T>::splitNode(T &key)
{
	//最小结点数量
    unsigned int minmumNodeNum = (degree - 1) / 2;
	//创建新结点
    TreeNode* newNode = new TreeNode(degree, this->isLeaf);

	/*
    if (newNode == NULL) {
        cout << "Problems in allocate momeory of TreeNode in splite node of " << key << endl;
        exit(2);
    }
	*/

	//当前结点为叶结点情况
    if (isLeaf) {
        key = keys[minmumNodeNum + 1];
		//将右半部分key值拷贝至新结点内
        for (unsigned int i = minmumNodeNum + 1; i < degree; i++) {
            newNode->keys[i-minmumNodeNum-1] = keys[i];
            keys[i] = T();
            newNode->vals[i-minmumNodeNum-1] = vals[i];
            vals[i] = int();
        }
		//将新结点放置到本结点右边
        newNode->nextLeafNode = this->nextLeafNode;
        this->nextLeafNode = newNode;
        newNode->parent = this->parent;

		//调整两结点内key数量
        newNode->num = minmumNodeNum;
        this->num = minmumNodeNum + 1;
    } else if (!isLeaf) {  //非叶结点情况
        key = keys[minmumNodeNum];
		//拷贝子结点指针至新结点
        for (unsigned int i = minmumNodeNum + 1; i < degree+1; i++) {
            newNode->childs[i-minmumNodeNum-1] = this->childs[i];
            newNode->childs[i-minmumNodeNum-1]->parent = newNode;
            this->childs[i] = NULL;
        }
		//拷贝key值至新结点
        for (unsigned int i = minmumNodeNum + 1; i < degree; i++) {
            newNode->keys[i-minmumNodeNum-1] = this->keys[i];
            this->keys[i] = T();
        }
		//调整结点相互位置关系
        this->keys[minmumNodeNum] = T();
        newNode->parent = this->parent;

		//调整结点内key数量
        newNode->num = minmumNodeNum;
        this->num = minmumNodeNum;
    }

    return newNode;
}

//输入：key值
//输出：添加的position
//功能：在枝干结点中添加key值，并返回添加的位置
template <class T>
unsigned int TreeNode<T>::addKey(T &key)
{
	//本结点内无key
    if (num == 0) {
        keys[0] = key;
        num ++;
        return 0;
    } else {
		//查找是否Key值已经存在
        unsigned int index = 0;
        bool exist = findKey(key, index);
        if (exist) {
			/*
            cout << "Error:In add(T &key),key has already in the tree!" << endl;
            exit(3);
			*/
        } else { //不存在，可以进行插入
			//调整其他key值
            for (unsigned int i = num; i > index; i--)
                keys[i] = keys[i-1];
            keys[index] = key;

			//调整子结点指针情况
            for (unsigned int i = num + 1; i > index+1; i--)
                childs[i] = childs[i-1];
            childs[index+1] = NULL;
            num++;

            return index;
        }
    }

    return 0;
}

//输入：key值
//输出：添加的position
//功能：在叶结点中添加key值，并返回添加的位置
template <class T>
unsigned int TreeNode<T>::addKey(T &key, int val)
{	//若非叶结点，无法插入
    if (!isLeaf) {
		/*
        cout << "Error:add(T &key,int val) is a function for leaf nodes" << endl;
		*/
        return -1;
    }

	//结点内没有key值
    if (num == 0) {
        keys[0] = key;
        vals[0] = val;
        num ++;
        return 0;
    } else { //正常插入
        unsigned int index = 0;
        bool exist = findKey(key, index);
        if (exist) {
			/*
	        cout << "Error:In add(T &key, int val),key has already in the tree!" << endl;
	        exit(3);
			*/
        } else {
			//逐个调整key值
            for (unsigned int i = num; i > index; i--) {
                keys[i] = keys[i-1];
                vals[i] = vals[i-1];
            }
            keys[index] = key;
            vals[index] = val;
            num++;
            return index;
        }
    }

    return 0;
}

//输入：key值对应的index
//输出：bool
//功能：在结点中删除index对应的key值
template <class T>
bool TreeNode<T>::deleteKeyByIndex(unsigned int index)
{
	//index错误，超过本结点范围
    if(index > num) {
		/*
        cout << "Error:In removeAt(unsigned int index), index is more than num!" << endl;
		*/
        return false;
    } else { //正常进行删除
        if (isLeaf) { //叶结点情况
			//逐个调整key值
            for (unsigned int i = index; i < num-1; i++) {
                keys[i] = keys[i+1];
                vals[i] = vals[i+1];
            }
            keys[num-1] = T();
            vals[num-1] = int();
        } else { //枝干结点情况
			//调整key值
            for(unsigned int i = index; i < num-1; i++)
                keys[i] = keys[i+1];

			//调整子结点指针
            for(unsigned int i = index+1;i < num;i ++)
                childs[i] = childs[i+1];

            keys[num-1] = T();
            childs[num] = NULL;
        }

        num--;
        return true;
    }

    return false;
}

//返回下一个相邻叶结点指针
template <class T>
TreeNode<T>* TreeNode<T>::nextLeaf()
{
	return nextLeafNode;
}

//输入，起始index,终止key，返回结果vector容器
//输出:到达终止key返回true，否则返回flase
//功能：返回一定范围的value容器
template <class T>
bool TreeNode<T>::findRange(unsigned int index, T& key, std::vector<int>& valsout)
{
	unsigned int i;
	for (i = index; i < num && keys[i] <= key; i++)
		valsout.push_back(vals[i]);

	if (keys[i] >= key)
		return true;
	else
		return false;
}

template <class T>
bool TreeNode<T>::findRange2(unsigned int index, std::vector<int>& valsout)
{
	unsigned int i;
	for (i = index; i < num; i++)
		valsout.push_back(vals[i]);

	return false;
}

/*   Class   BPlusTree   */

//构造函数
//用于构造一颗新的树，确定m_name,key的size，树的度
//初始化各个变量
//同时调用其他函数为本树分配内存
template <class T>
BPlusTree<T>::BPlusTree(std::string in_name, int keysize, int in_degree):
	file_name(in_name),
	key_num(0),
	level(0),
	node_num(0),
	root(NULL),
	leafHead(NULL),
	key_size(keysize),
	degree(in_degree)
{
	//初始化分配内存并从磁盘读取数据
	//创建索引
    initTree();
    readFromDiskAll();
}

//析构函数
//释放相应的内存
template <class T>
BPlusTree<T>:: ~BPlusTree()
{
    dropTree(root);
    key_num = 0;
    root = NULL;
    level = 0;
}

//初始化B+树，并分配内存空间
template <class T>
void BPlusTree<T>::initTree()
{
    root = new TreeNode<T>(degree, true);
    key_num = 0;
    level = 1;
    node_num = 1;
    leafHead = root;
}

//用于查找某key值所处的叶结点位置
template <class T>
void BPlusTree<T>::findToLeaf(Tree pNode, T key, searchNodeParse &snp)
{
    unsigned int index = 0;
	//在对应结点内查找key值
    if (pNode->findKey(key, index)) {
		//若此结点是叶结点，则查找成功
        if (pNode->isLeaf) {
            snp.pNode = pNode;
            snp.index = index;
            snp.ifFound = true;
        } else {
			//此结点不是子结点，查找它的下一层
            pNode = pNode->childs[index+1];
            while (!pNode->isLeaf) {
                pNode = pNode->childs[0];
            }
			//因为已找到key值，所以其最底层叶结点index[0]即为该key
            snp.pNode = pNode;
            snp.index = 0;
            snp.ifFound = true;
        }

    } else { //本结点内未找到该key
        if (pNode->isLeaf) {
			//若此结点已经是叶结点则查找失败
            snp.pNode = pNode;
            snp.index = index;
            snp.ifFound = false;
        } else {
			//递归寻找下一层
            findToLeaf(pNode->childs[index], key, snp);
        }
    }

	return;
}

//输入：key值及其value
//输出：bool
//功能：在树中插入一个key值
//返回是否插入成功
template <class T>
bool BPlusTree<T>::insertKey(T &key, int val)
{
    searchNodeParse snp;
	//根结点不存在
    if (!root)
		initTree();
	//查找插入值是否存在
    findToLeaf(root, key, snp);
    if (snp.ifFound) { //已存在
		/*
        cout << "Error:in insert key to index: the duplicated key!" << endl;
		*/
        return false;
    } else { //不存在，可以插入
        snp.pNode->addKey(key, val);
		//插入后结点满，需要进行调整
        if (snp.pNode->num == degree) {
            adjustAfterinsert(snp.pNode);
        }
        key_num++;
        return true;
    }

    return false;
}

//用于插入某key后若结点满，对整棵树进行调整
template <class T>
bool BPlusTree<T>::adjustAfterinsert(Tree pNode)
{
    T key;
    Tree newNode = pNode->splitNode(key);
    node_num++;

	//当前结点为根结点情况
    if (pNode->isRoot()) {
        Tree root = new TreeNode<T>(degree, false);
        if (root == NULL) {
			/*
            cout << "Error: can not allocate memory for the new root in adjustAfterinsert" << endl;
            exit(1);
			*/
        } else {
            level ++;
            node_num ++;
            this->root = root;
            pNode->parent = root;
            newNode->parent = root;
            root->addKey(key);
            root->childs[0] = pNode;
            root->childs[1] = newNode;
            return true;
        }
    } else { //当前结点非根结点
        Tree parent = pNode->parent;
        unsigned int index = parent->addKey(key);

        parent->childs[index+1] = newNode;
        newNode->parent = parent;
		//递归进行调整
        if(parent->num == degree)
            return adjustAfterinsert(parent);

        return true;
    }

    return false;
}

template <class T>
int BPlusTree<T>::searchVal(T& key)
{
    if (!root)
        return -1;
    searchNodeParse snp;
    findToLeaf(root, key, snp);

    if (!snp.ifFound)
        return -1;
    else
        return snp.pNode->vals[snp.index];
}

//输入：key值
//输出：bool
//功能：在树中删除一个key值
//返回是否删除成功
template <class T>
bool BPlusTree<T>::deleteKey(T &key)
{
    searchNodeParse snp;
	//根结点不存在
    if (!root) {
		/*
        cout << "ERROR: In deleteKey, no nodes in the tree " << fileName << "!" << endl;
		*/
        return false;
    } else { //正常进行操作
		//查找位置
        findToLeaf(root, key, snp);
        if (!snp.ifFound) { //找不到该key
			/*
            cout << "ERROR: In deleteKey, no keys in the tree " << fileName << "!" << endl;
			*/
            return false;
        } else { //正常找到进行删除
            if (snp.pNode->isRoot()) { //当前为根结点
                snp.pNode->deleteKeyByIndex(snp.index);
                key_num--;
                return adjustAfterDelete(snp.pNode);
            } else {
                if (snp.index == 0 && leafHead != snp.pNode) {
					//key存在于枝干结点上
					//到上一层去更新枝干层
                    unsigned int index = 0;

                    Tree now_parent = snp.pNode->parent;
                    bool if_found_inBranch = now_parent->findKey(key, index);
                    while (!if_found_inBranch) {
                        if(now_parent->parent)
                            now_parent = now_parent->parent;
                        else
                            break;
                        if_found_inBranch = now_parent->findKey(key,index);
                    }

                    now_parent -> keys[index] = snp.pNode->keys[1];

                    snp.pNode->deleteKeyByIndex(snp.index);
                    key_num--;
                    return adjustAfterDelete(snp.pNode);

                } else { //同时必然存在于叶结点
                    snp.pNode->deleteKeyByIndex(snp.index);
                    key_num--;
                    return adjustAfterDelete(snp.pNode);
                }
            }
        }
    }

    return false;
}

//用于删除某key后可能出现结点空情况，对整棵树进行调整
template <class T>
bool BPlusTree<T>::adjustAfterDelete(Tree pNode)
{
    unsigned int minmumKeyNum = (degree - 1) / 2;
	//三种不需要调整的情况
    if (((pNode->isLeaf) && (pNode->num >= minmumKeyNum)) ||
	    ((degree != 3) && (!pNode->isLeaf) && (pNode->num >= minmumKeyNum - 1)) ||
		((degree == 3) && (!pNode->isLeaf) && (pNode->num < 0))) {
        return  true;
    }
    if (pNode->isRoot()) { //当前结点为根结点
        if (pNode->num > 0) //不需要调整
            return true;
        else { //正常需要调整
            if (root->isLeaf) { //将成为空树情况
                delete pNode;
                root = NULL;
                leafHead = NULL;
                level--;
                node_num--;
            }
            else { //根节点将成为左头部
                root = pNode -> childs[0];
                root -> parent = NULL;
                delete pNode;
                level--;
                node_num--;
            }
        }
    } else { //非根节点情况
        Tree parent = pNode->parent, brother = NULL;
        if (pNode->isLeaf) { //当前为叶节点
            unsigned int index = 0;
            parent->findKey(pNode->keys[0], index);

			//选择左兄弟
            if ((parent->childs[0] != pNode) && (index + 1 == parent->num)) {
                brother = parent->childs[index];
                if(brother->num > minmumKeyNum) {
                    for (unsigned int i = pNode->num; i > 0; i--) {
                        pNode->keys[i] = pNode->keys[i-1];
                        pNode->vals[i] = pNode->vals[i-1];
                    }
                    pNode->keys[0] = brother->keys[brother->num-1];
                    pNode->vals[0] = brother->vals[brother->num-1];
                    brother->deleteKeyByIndex(brother->num-1);

                    pNode->num++;
                    parent->keys[index] = pNode->keys[0];
                    return true;

                } else {
                    parent->deleteKeyByIndex(index);

                    for (int i = 0; i < pNode->num; i++) {
                        brother->keys[i+brother->num] = pNode->keys[i];
                        brother->vals[i+brother->num] = pNode->vals[i];
                    }
                    brother->num += pNode->num;
                    brother->nextLeafNode = pNode->nextLeafNode;

                    delete pNode;
                    node_num--;

                    return adjustAfterDelete(parent);
                }

            } else {
                if(parent->childs[0] == pNode)
                    brother = parent->childs[1];
                else
                    brother = parent->childs[index+2];
                if(brother->num > minmumKeyNum) {
                    pNode->keys[pNode->num] = brother->keys[0];
                    pNode->vals[pNode->num] = brother->vals[0];
                    pNode->num++;
                    brother->deleteKeyByIndex(0);
                    if(parent->childs[0] == pNode)
                        parent->keys[0] = brother->keys[0];
                    else
                        parent->keys[index+1] = brother->keys[0];
                    return true;

                } else {
                    for (int i = 0; i < brother->num; i++) {
                        pNode->keys[pNode->num+i] = brother->keys[i];
                        pNode->vals[pNode->num+i] = brother->vals[i];
                    }
                    if (pNode == parent->childs[0])
                        parent->deleteKeyByIndex(0);
                    else
                        parent->deleteKeyByIndex(index+1);
                    pNode->num += brother->num;
                    pNode->nextLeafNode = brother->nextLeafNode;
                    delete brother;
                    node_num--;

                    return adjustAfterDelete(parent);
                }
            }

        } else { //枝干节点情况
            unsigned int index = 0;
            parent->findKey(pNode->childs[0]->keys[0], index);
            if ((parent->childs[0] != pNode) && (index + 1 == parent->num)) {
                brother = parent->childs[index];
                if (brother->num > minmumKeyNum - 1) {
                    pNode->childs[pNode->num+1] = pNode->childs[pNode->num];
                    for (unsigned int i = pNode->num; i > 0; i--) {
                        pNode->childs[i] = pNode->childs[i-1];
                        pNode->keys[i] = pNode->keys[i-1];
                    }
                    pNode->childs[0] = brother->childs[brother->num];
                    pNode->keys[0] = parent->keys[index];
                    pNode->num++;

                    parent->keys[index]= brother->keys[brother->num-1];

                    if (brother->childs[brother->num])
                        brother->childs[brother->num]->parent = pNode;
                    brother->deleteKeyByIndex(brother->num-1);

                    return true;

                } else {
                    brother->keys[brother->num] = parent->keys[index];
                    parent->deleteKeyByIndex(index);
                    brother->num++;

                    for (int i = 0; i < pNode->num; i++) {
                        brother->childs[brother->num+i] = pNode->childs[i];
                        brother->keys[brother->num+i] = pNode->keys[i];
                        brother->childs[brother->num+i]-> parent= brother;
                    }
                    brother->childs[brother->num+pNode->num] = pNode->childs[pNode->num];
                    brother->childs[brother->num+pNode->num]->parent = brother;

                    brother->num += pNode->num;

                    delete pNode;
                    node_num --;

                    return adjustAfterDelete(parent);
                }

            } else {
                if (parent->childs[0] == pNode)
                    brother = parent->childs[1];
                else
                    brother = parent->childs[index+2];
                if (brother->num > minmumKeyNum - 1) {

                    pNode->childs[pNode->num+1] = brother->childs[0];
                    pNode->keys[pNode->num] = brother->keys[0];
                    pNode->childs[pNode->num+1]->parent = pNode;
                    pNode->num++;

                    if (pNode == parent->childs[0])
                        parent->keys[0] = brother->keys[0];
                    else
                        parent->keys[index+1] = brother->keys[0];

                    brother->childs[0] = brother->childs[1];
                    brother->deleteKeyByIndex(0);

                    return true;
                } else {

                    pNode->keys[pNode->num] = parent->keys[index];

                    if(pNode == parent->childs[0])
                        parent->deleteKeyByIndex(0);
                    else
                        parent->deleteKeyByIndex(index+1);

                    pNode->num++;

                    for (int i = 0; i < brother->num; i++) {
                        pNode->childs[pNode->num+i] = brother->childs[i];
                        pNode->keys[pNode->num+i] = brother->keys[i];
                        pNode->childs[pNode->num+i]->parent = pNode;
                    }
                    pNode->childs[pNode->num+brother->num] = brother->childs[brother->num];
                    pNode->childs[pNode->num+brother->num]->parent = pNode;

                    pNode->num += brother->num;

                    delete brother;
                    node_num--;

                    return adjustAfterDelete(parent);
                }

            }

        }

    }

    return false;
}

//输入：树的根结点
//功能：删除整棵树并释放内存空间，主要用在析构函数中
template <class T>
void BPlusTree<T>::dropTree(Tree node)
{
	//空树
    if (!node)
		return;
	//非叶节点
    if (!node->isLeaf) {
        for(unsigned int i = 0; i <= node->num; i++) {
            dropTree(node->childs[i]);
            node->childs[i] = NULL;
        }
    }
    delete node;
    node_num--;
    return;
}

//输入：key1，key2，返回vals的容器
//功能：返回范围搜索结果，将value放入vals容器中
template <class T>
void BPlusTree<T>::searchRange(T& key1, T& key2, std::vector<int>& vals, int flag)
{
	//空树
	if (!root)
		return;

	if (flag == 2) {
		searchNodeParse snp1;
		findToLeaf(root, key1, snp1);

		bool finished = false;
		Tree pNode = snp1.pNode;
		unsigned int index = snp1.index;

		do {
			finished = pNode->findRange2(index, vals);
			index = 0;
			if (pNode->nextLeafNode == NULL)
				break;
			else
				pNode = pNode->nextLeaf();
		} while (!finished);
	} else if (flag == 1) {
		searchNodeParse snp2;
		findToLeaf(root, key2, snp2);

		bool finished = false;
		Tree pNode = snp2.pNode;
		unsigned int index = snp2.index;

		do {
			finished = pNode->findRange2(index, vals);
			index = 0;
			if (pNode->nextLeafNode == NULL)
				break;
			else
				pNode = pNode->nextLeaf();
		} while (!finished);
	} else {
		searchNodeParse snp1, snp2;
	    findToLeaf(root, key1, snp1);
		findToLeaf(root, key2, snp2);
		bool finished = false;
		unsigned int index;

		if (key1 <= key2) {
			Tree pNode = snp1.pNode;
			index = snp1.index;
			do {
				finished = pNode->findRange(index, key2, vals);
				index = 0;
				if (pNode->nextLeafNode == NULL)
					break;
				else
					pNode = pNode->nextLeaf();
			} while (!finished);
		} else {
			Tree pNode = snp2.pNode;
			index = snp2.index;
			do {
				finished = pNode->findRange(index, key1, vals);
				index = 0;
				if (pNode->nextLeafNode == NULL)
					break;
				else
					pNode = pNode->nextLeaf();
			} while (!finished);
		}
	}
	//}

	std::sort(vals.begin(),vals.end());
    vals.erase(unique(vals.begin(), vals.end()), vals.end());
	return;
}

//获取文件大小
template <class T>
void BPlusTree<T>::getFile(std::string fname) {
    FILE* f = fopen(fname.c_str() , "r");
    if (f == NULL) {
        f = fopen(fname.c_str(), "w+");
        fclose(f);
        f = fopen(fname.c_str() , "r");
    }
    fclose(f);
    return;
}

template <class T>
int BPlusTree<T>::getBlockNum(std::string table_name)
{
    char* p;
    int block_num = -1;
    do {
        p = buffer_manager.getPage(table_name , block_num + 1);
        block_num++;
    } while(p[0] != '\0');
    return block_num;
}

template <class T>
void BPlusTree<T>::readFromDiskAll()
{
    std::string fname = "./database/index/" + file_name;
    //std::string fname = file_name;
    getFile(fname);
    int block_num = getBlockNum(fname);

	if (block_num <= 0)
        block_num = 1;

	for (int i = 0; i < block_num; i++) {
        //获取当前块的句柄
        char* p = buffer_manager.getPage(fname, i);
        //char* t = p;
        //遍历块中所有记录

		readFromDisk(p, p+PAGESIZE);
    }
}


template <class T>
void BPlusTree<T>::readFromDisk(char* p, char* end)
{
    T key;
    int value;
    
    for (int i = 0; i < PAGESIZE; i++)
        if (p[i] != '#')
            return;
        else {
            i += 2;
            char tmp[100];
            int j;
            
            for (j = 0; i < PAGESIZE && p[i] != ' '; i++)
                tmp[j++] = p[i];
            tmp[j] = '\0';
            std::string s(tmp);
            std::stringstream stream(s);
            stream >> key;
            
            memset(tmp, 0, sizeof(tmp));
            
            i++;
            for (j = 0; i < PAGESIZE && p[i] != ' '; i++)
                tmp[j++] = p[i];
            tmp[j] = '\0';
            std::string s1(tmp);
            std::stringstream stream1(s1);
            stream1 >> value;
            
            insertKey(key, value);
        }
}


template <class T>
void BPlusTree<T>::writtenbackToDiskAll()
{
    std::string fname = "./database/index/" + file_name;
    //std::string fname = file_name;
    getFile(fname);
	int block_num = getBlockNum(fname);

    Tree ntmp = leafHead;
	int i, j;
    
    for (j = 0, i = 0; ntmp != NULL; j++) {
		char* p = buffer_manager.getPage(fname, j);
        int offset = 0;
        
		memset(p, 0, PAGESIZE);
        
        for (i = 0; i < ntmp->num; i++) {
            p[offset++] = '#';
            p[offset++] = ' ';
            
            copyString(p, offset, ntmp->keys[i]);
            p[offset++] = ' ';
            copyString(p, offset, ntmp->vals[i]);
            p[offset++] = ' ';
		}
        
        p[offset] = '\0';

		int page_id = buffer_manager.getPageId(fname, j);
		buffer_manager.modifyPage(page_id);
        
        ntmp = ntmp->nextLeafNode;
    }

    while (j < block_num) {
		char* p = buffer_manager.getPage(fname, j);
		memset(p, 0, PAGESIZE);

		int page_id = buffer_manager.getPageId(fname, j);
		buffer_manager.modifyPage(page_id);

        j++;
    }

	return;

}

template <class T>
void BPlusTree<T>::printleaf()
{
    Tree p = leafHead;
    while (p != NULL) {
        p->printl();
        p = p->nextLeaf();
    }
    
    return;
}

template <class T>
void TreeNode<T>::printl()
{
    for (int i = 0; i < num; i++)
        std::cout << "->" << keys[i];
    std::cout<<std::endl;
    
}
#endif
