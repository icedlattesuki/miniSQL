//
//  index_manager.cc
//  index_manager
//
//  Created by Xw on 2017/6/5.
//  Copyright © 2017年 Xw. All rights reserved.
//

#include "index_manager.h"
#include "const.h"
#include "basic.h"
#include "buffer_manager.h"
#include "bplustree.h"
#include "catalog_manager.h"
#include <string>
#include <vector>
#include <map>

IndexManager::IndexManager(std:: string table_name)
{
    CatalogManager catalog;
    Attribute attr = catalog.getAttribute(table_name);
    
    for (int i = 0; i < attr.num; i++)
        if (attr.has_index[i])
            createIndex("INDEX_FILE_" + attr.name[i] + "_" + table_name, attr.type[i]);
}

IndexManager::~IndexManager()
{
    for (intMap::iterator itInt = indexIntMap.begin(); itInt != indexIntMap.end(); itInt++) {
        if (itInt->second) {
            itInt->second->writtenbackToDiskAll();
            delete itInt->second;
        }
    }
    for (stringMap::iterator itString = indexStringMap.begin(); itString != indexStringMap.end(); itString++) {
        if (itString->second) {
            itString->second->writtenbackToDiskAll();
            delete itString->second;
        }
    }
    for (floatMap::iterator itFloat = indexFloatMap.begin(); itFloat != indexFloatMap.end(); itFloat++) {
        if(itFloat->second) {
            itFloat->second->writtenbackToDiskAll();
            delete itFloat->second;
        }
    }
}

void IndexManager::createIndex(std::string file_path, int type)
{
    int key_size = getKeySize(type); //获取key的size
    int degree = getDegree(type); //获取需要的degree

	//根据数据类型不同，用对应的方法建立映射关系
	//并且先初始化一颗B+树
    if (type == TYPE_INT) {
        BPlusTree<int> *tree = new BPlusTree<int>(file_path, key_size, degree);
        indexIntMap.insert(intMap::value_type(file_path, tree));
    } else if(type == TYPE_FLOAT) {
        BPlusTree<float> *tree = new BPlusTree<float>(file_path, key_size, degree);
        indexFloatMap.insert(floatMap::value_type(file_path, tree));
    } else {
        BPlusTree<std::string> *tree = new BPlusTree<std::string>(file_path, key_size, degree);
        indexStringMap.insert(stringMap::value_type(file_path, tree));
    }

	return;
}

void IndexManager::dropIndex(std::string file_path, int type)
{
	//根据不同数据类型采用对应的处理方式
    if (type == TYPE_INT) {
		//查找路径对应的键值对
        intMap::iterator itInt = indexIntMap.find(file_path);
        if (itInt == indexIntMap.end()) { //未找到
            // cout << "Error:in drop index, no index " << file_path <<" exits" << endl;
			return;
        } else {
            //删除对应的B+树
            delete itInt->second;
			//清空该键值对
            indexIntMap.erase(itInt);
        }
    } else if (type == TYPE_FLOAT) { //同上
        floatMap::iterator itFloat = indexFloatMap.find(file_path);
        if (itFloat == indexFloatMap.end()) {
            // cout << "Error:in drop index, no index " << file_path <<" exits" << endl;
            return;
        } else {
            delete itFloat->second;
            indexFloatMap.erase(itFloat);
        }
    } else {
        stringMap::iterator itString = indexStringMap.find(file_path);
        if (itString == indexStringMap.end()) { //同上
            // cout << "Error:in drop index, no index " << file_path <<" exits" << endl;
            return;
        } else {
            delete itString->second;
            indexStringMap.erase(itString);
        }
    }

	return;
}

int IndexManager::findIndex(std::string file_path, Data data)
{
    //setKey(type, key);

    if (data.type == TYPE_INT) {
        intMap::iterator itInt = indexIntMap.find(file_path);
        if (itInt == indexIntMap.end()) { //未找到
            // cout << "Error:in search index, no index " << file_path <<" exits" << endl;
            return -1;
        } else
            //找到则返回对应的键值
            return itInt->second->searchVal(data.datai);
    } else if(data.type == TYPE_FLOAT) {
        floatMap::iterator itFloat = indexFloatMap.find(file_path);
        if (itFloat == indexFloatMap.end()) { //同上
            // cout << "Error:in search index, no index " << file_path <<" exits" << endl;
            return -1;
        } else
            return itFloat->second->searchVal(data.dataf);
    } else {
        stringMap::iterator itString = indexStringMap.find(file_path);
        if (itString == indexStringMap.end()) { //同上
            // cout << "Error:in search index, no index " << file_path <<" exits" << endl;
            return -1;
        } else
            return itString->second->searchVal(data.datas);
    }
}

void IndexManager::insertIndex(std::string file_path, Data data, int block_id)
{
    //setKey(type, key);

    if (data.type == TYPE_INT) {
        intMap::iterator itInt = indexIntMap.find(file_path);
        if (itInt == indexIntMap.end()) {
            // cout << "Error:in search index, no index " << file_path <<" exits" << endl;
            return;
        } else
            itInt->second->insertKey(data.datai, block_id);
    } else if (data.type == TYPE_FLOAT) {
        floatMap::iterator itFloat = indexFloatMap.find(file_path);
        if (itFloat == indexFloatMap.end()) {
            // cout << "Error:in search index, no index " << file_path <<" exits" << endl;
			return;
        } else
            itFloat->second->insertKey(data.dataf, block_id);
    } else {
        stringMap::iterator itString = indexStringMap.find(file_path);
        if (itString == indexStringMap.end()) {
            // cout << "Error:in search index, no index " << file_path <<" exits" << endl;
            return;
        } else
            itString->second->insertKey(data.datas, block_id);
    }

    return;
}

void IndexManager::deleteIndexByKey(std::string file_path, Data data)
{
    //setKey(type, key);

    if (data.type == TYPE_INT) {
        intMap::iterator itInt = indexIntMap.find(file_path);
        if (itInt == indexIntMap.end()) {
            // cout << "Error:in search index, no index " << file_path <<" exits" << endl;
            return;
        } else
            itInt->second->deleteKey(data.datai);
    } else if (data.type == TYPE_FLOAT) {
        floatMap::iterator itFloat = indexFloatMap.find(file_path);
        if (itFloat == indexFloatMap.end()) {
            // cout << "Error:in search index, no index " << file_path <<" exits" << endl;
            return;
        } else
            itFloat->second->deleteKey(data.dataf);
    } else {
        stringMap::iterator itString = indexStringMap.find(file_path);
        if(itString == indexStringMap.end()) {
            // cout << "Error:in search index, no index " << file_path <<" exits" << endl;
            return;
        }
        else
			itString->second->deleteKey(data.datas);
    }
}

int IndexManager::getDegree(int type)
{
    int degree = (PAGESIZE - sizeof(int)) / (getKeySize(type) + sizeof(int));
    if (degree % 2 == 0)
		degree -= 1;
    return degree;
}

int IndexManager::getKeySize(int type)
{
    if (type == TYPE_FLOAT)
        return sizeof(float);
    else if (type == TYPE_INT)
        return sizeof(int);
    else if (type > 0)
        return type;
    else {
        // cout << "ERROR: in getKeySize: invalid type" << endl;
        return -100;
    }
}

void IndexManager::searchRange(std::string file_path, Data data1, Data data2, std::vector<int>& vals)
{
    int flag = 0;
    //检测数据类型是否匹配
    if (data1.type == -2) {
        flag = 1;
    } else if (data2.type == -2) {
        flag = 2;
    }
    /*
    else if (data1.type != data2.type) {
        // cout << "ERROR: in searchRange: Wrong data type!" << endl;
        return;
    }
     */
    
    if (data1.type == TYPE_INT) {
        intMap::iterator itInt = indexIntMap.find(file_path);
        if (itInt == indexIntMap.end()) {
            // cout << "Error:in search index, no index " << file_path <<" exits" << endl;
            return;
        } else
            itInt->second->searchRange(data1.datai, data2.datai, vals, flag);
    } else if (data1.type == TYPE_FLOAT) {
        floatMap::iterator itFloat = indexFloatMap.find(file_path);
        if (itFloat == indexFloatMap.end()) {
            // cout << "Error:in search index, no index " << file_path <<" exits" << endl;
            return;
        } else
            itFloat->second->searchRange(data1.dataf, data2.dataf, vals, flag);
    } else {
        stringMap::iterator itString = indexStringMap.find(file_path);
        if(itString == indexStringMap.end()) {
            // cout << "Error:in search index, no index " << file_path <<" exits" << endl;
            return;
        }
        else
            itString->second->searchRange(data1.datas, data2.datas, vals, flag);
    }
}

/*
void IndexManager::setKey(int type, std::tring key)
{
    stringstream ss;
    ss << key;
    if (type == this->TYPE_INT)
        ss >> this->kt.intTmp;
    else if (type == this->TYPE_FLOAT)
        ss >> this->kt.floatTmp;
    else if (type > 0)
        ss >> this->kt.stringTmp;

    else
        cout << "Error: in getKey: invalid type" << endl;


	return;
}
*/
