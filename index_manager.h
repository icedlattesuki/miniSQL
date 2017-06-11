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

class IndexManager {
public:
	//构造函数
	IndexManager();
	//析构函数
	~IndexManager();

	void createIndex(std::string file_path, int type);

    void dropIndex(std::string file_path, int type);

    int findIndex(std::string file_path, Data data);

    void insertIndex(std::string file_path, Data data , int blockOffset);

    void deleteIndexByKey(std::string file_path, Data data);

private:
	typedef map<string,BPlusTree<int> *> intMap;
	typedef map<string,BPlusTree<string> *> stringMap;
	typedef map<string,BPlusTree<float> *> floatMap;

	int static const TYPE_FLOAT = 0;
	int static const TYPE_INT = -1;

	intMap indexIntMap;
	stringMap indexStringMap;
	floatMap indexFloatMap;
	struct keyTmp{
		int intTmp;
		float floatTmp;
		string stringTmp;
	};
	struct keyTmp kt;

	int getDegree(int type);

	int getKeySize(int type);

	void setKey(int type, std::string key);
};

#endif
