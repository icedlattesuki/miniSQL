//
//  api.cc
//  api
//
//  Created by Xw on 2017/5/31.
//  Copyright © 2017年 Xw. All rights reserved.
//
#include "api.h"
#include "template_function.h"
#include <algorithm>
#include <vector>

//构造函数
API::API(){}

//析构函数
API::~API(){}

//输入：表名、Where条件属性名、Where条件值域
//输出：Table类型对象(包含对应的属性元组)
//功能：返回包含所有目标属性满足Where条件的记录的表
//在多条件查询情况下，根据Where下的逻辑条件进行Table的拼接
//异常：由底层处理
//如果表不存在，抛出table_not_exist异常
//如果属性不存在，抛出attribute_not_exist异常
//如果Where条件中的两个数据类型不匹配，抛出data_type_conflict异常
Table API::selectRecord(std::string table_name, std::vector<std::string> target_attr, std::vector<Where> where, char operation)
{
	if (target_attr.size() == 0) {
		return record.selectRecord(table_name);
	} else {
		Table table1 = record.selectRecord(table_name, target_attr.at(0), where.at(0));
		Table table2 = record.selectRecord(table_name, target_attr.at(1), where.at(1));

		if (operation)
			return joinTable(table1, table2, target_attr.at(1), where.at(1));
		else
			return unionTable(table1, table2, target_attr.at(1), where.at(1));
	}
}
//输入：表名、Where条件属性名、Where条件值域
//输出：void
//功能：删除对应条件下的Table内记录(不删除表文件)
//异常：由底层处理
//如果表不存在，抛出table_not_exist异常
//如果属性不存在，抛出attribute_not_exist异常
//如果Where条件中的两个数据类型不匹配，抛出data_type_conflict异常
void API::deleteRecord(std::string table_name , std::string target_attr , Where where)
{
	if (target_attr == 0)
		record.deleteRecord(table_name);
	else
		record.deleteRecord(table_name, target_attr, where);
	return;
}
//输入：表名、一个元组对象
//输出：void
//功能：向对应表内插入一条记录
//异常：由底层处理
//如果元组类型不匹配，抛出tuple_type_conflict异常
//如果主键冲突，抛出primary_key_conflict异常
//如果unique属性冲突，抛出unique_conflict异常
//如果表不存在，抛出table_not_exist异常
void API::insertRecord(std::string table_name , Tuple& tuple)
{
	record.insertRecord(table_name, tuple);
	return;
}
//输入：Table类型对象
//输出：是否创建成功
//功能：在数据库中插入一个表的元信息
//异常：由底层处理
//如果已经有相同表名的表存在，则抛出table_exist异常
bool API::createTable(std::string table_name, Attribute attribute, int primary, Index index)
{
	record.createTableFile(table_name);
	catalog.createTable(table_name, attribute, primary, index);

	return true;
}
//输入：表名
//输出：是否删除成功
//功能：在数据库中删除一个表的元信息，及表内所有记录(删除表文件)
//异常：由底层处理
//如果表不存在，抛出table_not_exist异常
bool API::dropTable(std::string table_name)
{
	record.dropTableFile(table_name);
	catalog.dropTable(table_name);

	return true;
}
//输入：表名，索引名，属性名
//输出：是否创建成功
//功能：在数据库中更新对应表的索引信息（在指定属性上建立一个索引）
//异常：由底层处理
//如果表不存在，抛出table_not_exist异常
//如果对应属性不存在，抛出attribute_not_exist异常
//如果对应属性已经有了索引，抛出index_exist异常
bool API::createIndex(std::string table_name, std::string index_name, std::string attr_name)
{
	catalog.createIndex(table_name, index_name, attr_name);

	//
}
//输入：表名，索引名
//输出：是否删除成功
//功能：删除对应表的对应属性上的索引
//异常：由底层处理
//如果表不存在，抛出table_not_exist异常
//如果对应属性不存在，抛出attribute_not_exist异常
//如果对应属性没有索引，抛出index_not_exist异常
bool API::dropIndex(std::string table_name, std::string index_name)
{
	catalog.dropIndex(table_name, index_name);

	//
}

//私有函数，用于多条件查询时的or条件合并
Table API::unionTable(Table &table1, Table &table2)
{
	Table table_result;
	std::vector<Tuple> tuple1 = table1.getTuple();
	std::vector<Tuple> tuple2 = table2.getTuple();
	std::vector<Tuple> result_tuple = tuple1;

	for (int i = 0; i < table2.size(); i++) {
		if (isSatisfied())
	}



}
//私有函数，用于多条件查询时的and条件合并
Table API::joinTable(Table &table1, Table &table2)
{

}
