//
//  catalog.hpp
//  catalog
//
//  Created by Sr on 2017/5/24.
//  Copyright © 2017年 Sr. All rights reserved.
//

#ifndef _CATALOG_MANAGER_H_
#define _CATALOG_MANAGER_H_ 1

#include <iostream>
#include <cmath>
#include <cstring>
#include <iomanip>
#include "buffer_manager.h"
#include "basic.h"
#include "exception.h"
#include "const.h"

#ifndef TABLE_MANAGER_PATH
#define TABLE_MANAGER_PATH "./database/catalog/catalog_file"
#endif

extern BufferManager buffer_manager;

class CatalogManager{
public:
    //输入：表名，属性对象，主键编号，索引对象
    //输出：void
    //功能：在catalog文件中插入一个表的元信息
    //异常：如果已经有相同表名的表存在，则抛出table_exist异常
    void createTable(std::string table_name, Attribute attribute, int primary, Index index);
    //输入：表名
    //输出：void
    //功能：在catalog文件中删除一个表的元信息
    //异常：如果表不存在，抛出table_not_exist异常
    void dropTable(std::string table_name);
    //输入：表名
    //输出：bool
    //功能：查找对应表是否存在，存在返回true，不存在返回false
    //异常：无异常
    bool hasTable(std::string table_name);
    //输入：表名，属性名
    //输出：bool
    //功能：查找对应表中是否有某一属性，如果有返回true，如果没有返回false
    //异常：如果表不存在，抛出table_not_exist异常
    bool hasAttribute(std::string table_name , std::string attr_name);
    //输入：表名
    //输出：属性对象
    //功能：获取一个表的属性
    //异常：如果表不存在，抛出table_not_exist异常
    Attribute getAttribute(std::string table_name);
    //输入：表名，属性名，索引名
    //输出：void
    //功能：在catalog文件中更新对应表的索引信息（在指定属性上建立一个索引）
    //异常：如果表不存在，抛出table_not_exist异常。如果对应属性不存在，抛出attribute_not_exist异常。
    //如果对应属性已经有了索引，抛出index_exist异常。
    void createIndex(std::string table_name,std::string attr_name,std::string index_name);
    //输入：表名，索引名
    //输出：索引所对应的属性名
    //功能：通过索引名定位属性名
    //异常：如果表不存在，抛出table_not_exist异常。如果对应索引不存在，抛出index_not_exist异常。
    std::string IndextoAttr(std::string table_name,std::string index_name);
    //输入：表名，索引名
    //输出：void
    //功能：删除对应表的对应属性上的索引
    //异常：如果表不存在，抛出table_not_exist异常。如果对应属性不存在，抛出attribute_not_exist异常。
    //如果对应属性没有索引，抛出index_not_exist异常。
    void dropIndex(std::string table_name,std::string index_name);
    //输入：表名
    //输出：void
    //功能：显示表的信息
    //异常：如果表不存在，抛出table_not_exist异常
    void showTable(std::string table_name);
private:
    //数字转字符串，bit为数的位数
    std::string num2str(int num,short bit);
    //字符串转数字
    int str2num(std::string str);
    //得到该行的表的名字
    std::string getTableName(std::string buffer,int start,int &rear);
    //返回表在文件中的位置,返回具体位置，引用传出数据所在的块信息
    int getTablePlace(std::string name,int &suitable_block);
    //返回该表的index
    Index getIndex(std::string table_name);
    //获取文件大小
    int getBlockNum(std::string table_name);
};

#endif /* catalog_hpp */
