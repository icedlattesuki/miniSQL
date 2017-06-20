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
#include <iterator>

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
	} else if (target_attr.size() == 1) {
        return record.selectRecord(table_name, target_attr[0], where[0]);
    } else {
		Table table1 = record.selectRecord(table_name, target_attr[0], where[0]);
		Table table2 = record.selectRecord(table_name, target_attr[1], where[1]);

		if (operation)
			return joinTable(table1, table2, target_attr[0], where[0]);
		else
			return unionTable(table1, table2, target_attr[0], where[0]);
	}
}
//输入：表名、Where条件属性名、Where条件值域
//输出：void
//功能：删除对应条件下的Table内记录(不删除表文件)
//异常：由底层处理
//如果表不存在，抛出table_not_exist异常
//如果属性不存在，抛出attribute_not_exist异常
//如果Where条件中的两个数据类型不匹配，抛出data_type_conflict异常
int API::deleteRecord(std::string table_name , std::string target_attr , Where where)
{
    int result;
	if (target_attr == "")
		result = record.deleteRecord(table_name);
	else
		result = record.deleteRecord(table_name, target_attr, where);
	return result;
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
    IndexManager index(table_name);
    
	std::string file_path = "INDEX_FILE_" + attr_name + "_" + table_name;
	int type;

	catalog.createIndex(table_name, attr_name, index_name);
	Attribute attr = catalog.getAttribute(table_name);
	for (int i = 0; i < attr.num; i++) {
		if (attr.name[i] == attr_name) {
			type = (int)attr.type[i];
			break;
		}
	}
	index.createIndex(file_path, type);
	record.createIndex(index , table_name, attr_name);

	return true;
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
    IndexManager index(table_name);
    
	std::string attr_name = catalog.IndextoAttr(table_name, index_name);
	std::string file_path = "INDEX_FILE_" + attr_name + "_" + table_name;
	int type;

	Attribute attr = catalog.getAttribute(table_name);
	for (int i = 0; i < attr.num; i++) {
		if (attr.name[i] == attr_name) {
			type = (int)attr.type[i];
			break;
		}
	}
	index.dropIndex(file_path, type);
	catalog.dropIndex(table_name, index_name);
	
	file_path = "./database/index/" + file_path;
    remove(file_path.c_str());
	return true;
}

//私有函数，用于多条件查询时的or条件合并
Table API::unionTable(Table &table1, Table &table2, std::string target_attr, Where where)
{
	Table result_table(table1);
    std::vector<Tuple>& result_tuple = result_table.getTuple();
	std::vector<Tuple> tuple1 = table1.getTuple();
	std::vector<Tuple> tuple2 = table2.getTuple();
    result_tuple = tuple1;

	//std::vector<Tuple>().swap(result_tuple);

    int i;
    Attribute attr = table1.getAttr();
    for (i = 0; i < 32; i++)
        if (attr.name[i] == target_attr)
            break;
    
    for (int j = 0; j < tuple2.size(); j++)
        if (!isSatisfied(tuple2[j], i, where))
            result_tuple.push_back(tuple2[j]);
    
    std::sort(result_tuple.begin(), result_tuple.end(), sortcmp);
    return result_table;
	//std::merge(tuple1.begin(), tuple1.end(), tuple2.begin(), tuple2.end(), std::back_inserter(result_tuple), sortcmp);
    //result_tuple.erase(unique(result_tuple.begin(), result_tuple.end(), calcmp), result_tuple.end());
    
//    int i;
//    for (i = 0; i < tuple1.size() && i < tuple2.size(); i++) {
//        std::vector<Data> data1 = tuple1[i].getData();
//        std::vector<Data> data2 = tuple2[i].getData();
//        
//        int j;
//        for (j = 0; j < data1.size(); j++) {
//            int flag = 0;
//
//            switch (data1[i].type) {
//                case -1: {
//                    if (data1[i].datai != data2[i].datai) {
//                        if (data1[i].datai < data2[i].datai)
//                            flag = 1;
//                        else
//                            flag = -1;
//                    }
//                } break;
//                case 0: {
//                    if (data1[i].dataf != data2[i].dataf) {
//                        if (data1[i].dataf < data2[i].dataf)
//                            flag = 1;
//                        else
//                            flag = -1;
//                    }
//                } break;
//                default: {
//                    if (data1[i].datas < data2[i].datas)
//                        flag = 1;
//                    else
//                        flag = -1;
//                } break;
//            }
//            
//            if (flag)
//                break;
//        }
//        
//        switch (flag) {
//            case 1:
//        }
//    }
}

//私有函数，用于多条件查询时的and条件合并
Table API::joinTable(Table &table1, Table &table2, std::string target_attr, Where where)
{
	Table result_table(table1);
    std::vector<Tuple>& result_tuple = result_table.getTuple();
	std::vector<Tuple> tuple1 = table1.getTuple();
	std::vector<Tuple> tuple2 = table2.getTuple();
    
    int i;
    Attribute attr = table1.getAttr();
    for (i = 0; i < 32; i++)
        if (attr.name[i] == target_attr)
            break;
    
    for (int j = 0; j < tuple2.size(); j++)
        if (isSatisfied(tuple2[j], i, where))
            result_tuple.push_back(tuple2[j]);
    
    std::sort(result_tuple.begin(), result_tuple.end(), sortcmp);
    return result_table;
//	std::vector<Tuple>().swap(result_tuple);
//	std::sort(tuple1.begin(), tuple1.end(), sortcmp);
//	std::sort(tuple2.begin(), tuple2.end(), sortcmp);

    //std::set_intersection(tuple1.begin(), tuple1.end(), tuple2.begin(), tuple2.end(), std::back_inserter(result_tuple), calcmp);

	//return result_table;
}

//用于对vector的sort时排序
bool sortcmp(const Tuple &tuple1, const Tuple &tuple2)
{
	std::vector<Data> data1 = tuple1.getData();
	std::vector<Data> data2 = tuple2.getData();

    switch (data1[0].type) {
        case -1: return data1[0].datai < data2[0].datai;
        case 0: return data1[0].dataf < data2[0].dataf;
        default: return data1[0].datas < data2[0].datas;
    }
}

//用于对vector对合并时排序
bool calcmp(const Tuple &tuple1, const Tuple &tuple2)
{
	int i;

	std::vector<Data> data1 = tuple1.getData();
	std::vector<Data> data2 = tuple2.getData();

    for (i = 0; i < data1.size(); i++) {
        bool flag = false;
        switch (data1[0].type) {
            case -1: {
                if (data1[0].datai != data2[0].datai)
                    flag = true;
            }break;
            case 0: {
                if (data1[0].dataf != data2[0].dataf)
                    flag = true;
            }break;
            default: {
                if (data1[0].datas != data2[0].datas)
                    flag = true;
            }break;
        }
		if (flag)
			break;
    }


	if (i == data1.size())
		return true;
	else
		return false;
}

bool isSatisfied(Tuple& tuple, int target_attr, Where where)
{
    std::vector<Data> data = tuple.getData();
    
    switch (where.relation_character) {
        case LESS : {
            switch (where.data.type) {
                case -1 : return (data[target_attr].datai < where.data.datai); break;
                case 0 : return (data[target_attr].dataf < where.data.dataf); break;
                default: return (data[target_attr].datas < where.data.datas); break;
            }
        } break;
        case LESS_OR_EQUAL : {
            switch (where.data.type) {
                case -1 : return (data[target_attr].datai <= where.data.datai); break;
                case 0 : return (data[target_attr].dataf <= where.data.dataf); break;
                default: return (data[target_attr].datas <= where.data.datas); break;
            }
        } break;
        case EQUAL : {
            switch (where.data.type) {
                case -1 : return (data[target_attr].datai == where.data.datai); break;
                case 0 : return (data[target_attr].dataf == where.data.dataf); break;
                default: return (data[target_attr].datas == where.data.datas); break;
            }
        } break;
        case GREATER_OR_EQUAL : {
            switch (where.data.type) {
                case -1 : return (data[target_attr].datai >= where.data.datai); break;
                case 0 : return (data[target_attr].dataf >= where.data.dataf); break;
                default: return (data[target_attr].datas >= where.data.datas); break;
            }
        } break;
        case GREATER : {
            switch (where.data.type) {
                case -1 : return (data[target_attr].datai > where.data.datai); break;
                case 0 : return (data[target_attr].dataf > where.data.dataf); break;
                default: return (data[target_attr].datas > where.data.datas); break;
            }
        } break;
        case NOT_EQUAL : {
            switch (where.data.type) {
                case -1 : return (data[target_attr].datai != where.data.datai); break;
                case 0 : return (data[target_attr].dataf != where.data.dataf); break;
                default: return (data[target_attr].datas != where.data.datas); break;
            }
        } break;
        default:break;
    }
            
    return false;
}
