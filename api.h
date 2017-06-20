//
//  api.h
//  api
//
//  Created by Xw on 2017/5/31.
//  Copyright © 2017年 Xw. All rights reserved.
//
#ifndef _API_H_
#define _API_H_ 1

#include "basic.h"
#include "record_manager.h"
#include "buffer_manager.h"

//API接口。作为Interpreter层操作和底层Manager连接的接口
//包含程序的所有功能接口
//API只做初步的程序功能判断，具体的异常抛出仍由各底层Manager完成
class API {
public:
	//构造函数
	API();
	//析构函数
	~API();
	//输入：表名、Where条件属性名、Where条件值域
	//输出：Table类型对象(包含对应的属性元组)
	//功能：返回包含所有目标属性满足Where条件的记录的表
	//在多条件查询情况下，根据Where下的逻辑条件进行Table的拼接
	//异常：由底层处理
	//如果表不存在，抛出table_not_exist异常
	//如果属性不存在，抛出attribute_not_exist异常
	//如果Where条件中的两个数据类型不匹配，抛出data_type_conflict异常
	Table selectRecord(std::string table_name, std::vector<std::string> target_attr, std::vector<Where> where, char operation);
	//输入：表名、Where条件属性名、Where条件值域
	//输出：void
	//功能：删除对应条件下的Table内记录(不删除表文件)
	//异常：由底层处理
	//如果表不存在，抛出table_not_exist异常
	//如果属性不存在，抛出attribute_not_exist异常
	//如果Where条件中的两个数据类型不匹配，抛出data_type_conflict异常
	int deleteRecord(std::string table_name, std::string target_attr , Where where);
	//输入：表名、一个元组对象
	//输出：void
	//功能：向对应表内插入一条记录
	//异常：由底层处理
	//如果元组类型不匹配，抛出tuple_type_conflict异常
	//如果主键冲突，抛出primary_key_conflict异常
	//如果unique属性冲突，抛出unique_conflict异常
	//如果表不存在，抛出table_not_exist异常
	void insertRecord(std::string table_name, Tuple& tuple);
	//输入：表名，属性，主键，索引
    //输出：是否创建成功
    //功能：在数据库中插入一个表的元信息
    //异常：由底层处理
	//如果已经有相同表名的表存在，则抛出table_exist异常
	bool createTable(std::string table_name, Attribute attribute, int primary, Index index);
	//输入：表名
    //输出：是否删除成功
    //功能：在数据库中删除一个表的元信息，及表内所有记录(删除表文件)
    //异常：由底层处理
	//如果表不存在，抛出table_not_exist异常
	bool dropTable(std::string table_name);
	//输入：表名，索引名，属性名
    //输出：是否创建成功
    //功能：在数据库中更新对应表的索引信息（在指定属性上建立一个索引）
    //异常：由底层处理
	//如果表不存在，抛出table_not_exist异常
	//如果对应属性不存在，抛出attribute_not_exist异常
    //如果对应属性已经有了索引，抛出index_exist异常
	bool createIndex(std::string table_name, std::string index_name, std::string attr_name);
	//输入：表名，索引名
    //输出：是否删除成功
    //功能：删除对应表的对应属性上的索引
    //异常：由底层处理
	//如果表不存在，抛出table_not_exist异常
	//如果对应属性不存在，抛出attribute_not_exist异常
    //如果对应属性没有索引，抛出index_not_exist异常
	bool dropIndex(std::string table_name, std::string index_name);
private:
	//私有函数，用于多条件查询时的and条件合并
    Table unionTable(Table &table1, Table &table2, std::string target_attr, Where where);
	//私有函数，用于多条件查询时的or条件合并
	Table joinTable(Table &table1, Table &table2, std::string target_attr, Where where);

private:
	RecordManager record;
	CatalogManager catalog;
};

//用于对vector的sort时排序
bool sortcmp(const Tuple &tuple1, const Tuple &tuple2);
//用于对vector对合并时对排序
bool calcmp(const Tuple &tuple1, const Tuple &tuple2);
bool isSatisfied(Tuple& tuple, int target_attr, Where where);
#endif
