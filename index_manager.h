//
//  index_manager.h
//  index_manager
//
//  Created by Xw on 2017/6/5.
//  Copyright © 2017年 Xw. All rights reserved.
//

#ifndef _INDEX_MANAGER_H_
#define _INDEX_MANAGER_H_ 1

#include <sstream>
#include <string>
#include <map>
#include "basic.h"
#include "buffer_manager.h"
#include "bplustree.h"

class IndexManager {
public:
	//构造函数
    IndexManager(std::string table_name);
	//析构函数
	~IndexManager();
	//输入：Index文件名(路径)，索引的key的类型
	//输出：void
	//功能：创建索引文件及B+树
	//异常：
	void createIndex(std::string file_path, int type);
	//输入：Index文件名(路径)，索引的key的类型
	//输出：void
	//功能：删除索引、B+树及文件
	//异常：
    void dropIndex(std::string file_path, int type);
	//输入：Index文件名(路径)，索引的key(包含类型)
	//输出：void
	//功能：创建索引文件及B+树
	//异常：
    int findIndex(std::string file_path, Data data);
	//输入：Index文件名(路径)，索引的key(包含类型)，block_id
	//输出：void
	//功能：在指定索引中插入一个key
	//异常：
    void insertIndex(std::string file_path, Data data , int block_id);
	//输入：Index文件名(路径)，索引的key(包含类型)
	//输出：void
	//功能：在索引中删除相应的Key
	//异常：
    void deleteIndexByKey(std::string file_path, Data data);
	//输入：Index文件名(路径)，索引的key1(包含类型)，索引的key2(包含类型)，返回的vector引用
	//输出：void
	//功能：范围查找，返回一定范围内的value
	//异常：
	void searchRange(std::string file_path, Data data1, Data data2, std::vector<int>& vals);

private:
	typedef std::map<std::string, BPlusTree<int> *> intMap;
	typedef std::map<std::string, BPlusTree<std::string> *> stringMap;
	typedef std::map<std::string, BPlusTree<float> *> floatMap;

	int static const TYPE_FLOAT = 0;
	int static const TYPE_INT = -1;

	intMap indexIntMap;
	stringMap indexStringMap;
	floatMap indexFloatMap;

	// struct keyTmp{
	// 	int intTmp;
	// 	float floatTmp;
	// 	string stringTmp;
	// };
	// struct keyTmp kt;

	//计算B+树适合的degree
	int getDegree(int type);

	//计算不同类型Key的size
	int getKeySize(int type);

	//void setKey(int type, std::string key);
};

#endif
