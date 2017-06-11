//
//  index_manager.cc
//  index_manager
//
//  Created by Xw on 2017/6/5.
//  Copyright © 2017年 Xw. All rights reserved.
//

#include "index_manager.h"
#include <string>
#include <vector>
#include <map>

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
			/*
            cout << "Error:in drop index, no index " << file_path <<" exits" << endl;
			*/
			return;
        } else { //删除对应的B+树
            delete itInt->second;
			//清空该键值对
            indexIntMap.erase(itInt);
        }
    } else if (type == TYPE_FLOAT) { //同上
        floatMap::iterator itFloat = indexFloatMap.find(file_path);
        if (itFloat == indexFloatMap.end()) {
			/*
            cout << "Error:in drop index, no index " << file_path <<" exits" << endl;
			*/
            return;
        } else {
            delete itFloat->second;
            indexFloatMap.erase(itFloat);
        }
    } else {
        stringMap::iterator itString = indexStringMap.find(file_path);
        if (itString == indexStringMap.end()) { //同上
			/*
            cout << "Error:in drop index, no index " << file_path <<" exits" << endl;
			*/
            return;
        } else {
            delete itString->second;
            indexStringMap.erase(itString);
        }
    }

	return;
}

int IndexManager::findIndex(std::string file_path, std::string key, int type)
{
    setKey(type, key);

    if (type == TYPE_INT) {
        intMap::iterator itInt = indexIntMap.find(file_path);
        if (itInt == indexIntMap.end()) {
			/*
            cout << "Error:in search index, no index " << file_path <<" exits" << endl;
			*/
            return -1;
        }
        else
            return itInt->second->findKey(kt.intTmp);
    } else if(type == TYPE_FLOAT) {
        floatMap::iterator itFloat = indexFloatMap.find(file_path);
        if (itFloat == indexFloatMap.end()) {
			/*
            cout << "Error:in search index, no index " << file_path <<" exits" << endl;
			*/
            return -1;
        } else
            return itFloat->second->findKey(kt.floatTmp);
    } else {
        stringMap::iterator itString = indexStringMap.find(file_path);
        if (itString == indexStringMap.end()) {
			/*
            cout << "Error:in search index, no index " << file_path <<" exits" << endl;
			*/
            return -1;
        } else
            return itString->second->findKey(key);
    }
}

void IndexManager::insertIndex(std::string file_path, std::string key, int block_id, int type)
{
    setKey(type, key);

    if (type == TYPE_INT) {
        intMap::iterator itInt = indexIntMap.find(file_path);
        if (itInt == indexIntMap.end()) {
			/*
            cout << "Error:in search index, no index " << file_path <<" exits" << endl;
			*/
            return;
        } else
            itInt->second->insertKey(kt.intTmp, block_id);
    } else if (type == TYPE_FLOAT) {
        floatMap::iterator itFloat = indexFloatMap.find(file_path);
        if (itFloat == indexFloatMap.end()) {
			/*
            cout << "Error:in search index, no index " << file_path <<" exits" << endl;
			*/
			return;
        } else {
            itFloat->second->insertKey(kt.floatTmp, block_id);
    } else {
        stringMap::iterator itString = indexStringMap.find(file_path);
        if (itString == indexStringMap.end()) {
			/*
            cout << "Error:in search index, no index " << file_path <<" exits" << endl;
			*/
            return;
        } else
            itString->second->insertKey(key, block_id);
    }
}

void IndexManager::deleteIndexByKey(std::string file_path, std::string key, int type)
{
    setKey(type, key);

    if (type == TYPE_INT) {
        intMap::iterator itInt = indexIntMap.find(file_path);
        if (itInt == indexIntMap.end()) {
			/*
            cout << "Error:in search index, no index " << file_path <<" exits" << endl;
			*/
            return;
        } else
            itInt->second->deleteKey(kt.intTmp);
    } else if (type == TYPE_FLOAT) {
        floatMap::iterator itFloat = indexFloatMap.find(file_path);
        if (itFloat == indexFloatMap.end()) {
			/*
            cout << "Error:in search index, no index " << file_path <<" exits" << endl;
			*/
            return;
        } else
            itFloat->second->deleteKey(kt.floatTmp);
    } else {
        stringMap::iterator itString = indexStringMap.find(file_path);
        if(itString == indexStringMap.end()) {
			/*
            cout << "Error:in search index, no index " << file_path <<" exits" << endl;
			*/
            return;
        }
        else
			itString->second->deleteKey(key);
    }
}

int IndexManager::getDegree(int type)
{
    int degree = bm.getBlockSize() / (getKeySize(type)+sizeof(int));
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
        return type + 1;
    else {
		/*
        cout << "ERROR: in getKeySize: invalid type" << endl;
		*/
        return -100;
    }
}

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
	/*
    else
        cout << "Error: in getKey: invalid type" << endl;
	*/

	return;
}
