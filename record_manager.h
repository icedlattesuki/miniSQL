//
//  record_manager.h
//  record_manager
//
//  Created by Hys on 2017/5/30.
//  Copyright © 2017年 Hys. All rights reserved.
//
#ifndef _RECORD_MANAGER_H_
#define _RECORD_MANAGER_H_ 1
#define INF 1000000

#include <cstdio> 
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "basic.h"
#include "index_manager.h"
#include "catalog_manager.h"
#include "buffer_manager.h"
#include "exception.h"
#include "const.h"
#include "template_function.h"

extern BufferManager buffer_manager;

class RecordManager {
    public:
        //输入：表名
        //输出：void
        //功能：建立表文件
        //异常：无异常处理（由catalog manager处理）
        void createTableFile(std::string table_name);   
        //输入：表名
        //输出：void
        //功能：删除表文件
        //异常：无异常处理（由catalog manager处理）
        void dropTableFile(std::string table_name);     
        //输入：表名，一个元组
        //输出：void
        //功能：向对应表中插入一条记录
        //异常：如果元组类型不匹配，抛出tuple_type_conflict异常。如果
        //主键冲突，抛出primary_key_conflict异常。如果unique属性冲突，
        //抛出unique_conflict异常。如果表不存在，抛出table_not_exist异常。
        void insertRecord(std::string table_name , Tuple& tuple); 
        //输入：表名
        //输出：int(删除的记录数)
        //功能：删除对应表中所有记录（不删除表文件）
        //异常：如果表不存在，抛出table_not_exist异常
        int deleteRecord(std::string table_name);
        //输入：表名，目标属性，一个Where类型的对象
        //输出：int(删除的记录数)
        //功能：删除对应表中所有目标属性值满足Where条件的记录
        //异常：如果表不存在，抛出table_not_exist异常。如果属性不存在，抛出attribute_not_exist异常。
        //如果Where条件中的两个数据类型不匹配，抛出data_type_conflict异常。
        int deleteRecord(std::string table_name , std::string target_attr , Where where);
        //输入：表名
        //输出：Table类型对象
        //功能：返回整张表
        //异常：如果表不存在，抛出table_not_exist异常
        Table selectRecord(std::string table_name , std::string result_table_name = "tmp_table");
        //输入：表名，目标属性，一个Where类型的对象
        //输出：Table类型对象
        //功能：返回包含所有目标属性满足Where条件的记录的表
        //异常：如果表不存在，抛出table_not_exist异常。如果属性不存在，抛出attribute_not_exist异常。
        //如果Where条件中的两个数据类型不匹配，抛出data_type_conflict异常。
        Table selectRecord(std::string table_name , std::string target_attr , Where where , std::string result_table_name = "tmp_table");
        //输入：表名，目标属性名
        //输出：void
        //功能：对表中已经存在的记录建立索引
        //异常：如果表不存在，抛出table_not_exist异常。如果属性不存在，抛出attribute_not_exist异常。
        void createIndex(IndexManager& index_manager , std::string table_name , std::string target_attr);
    private:
        //获取文件大小
        int getBlockNum(std::string table_name);
        //insertRecord的辅助函数
        void insertRecord1(char* p , int offset , int len , const std::vector<Data>& v);
        //deleteRecord的辅助函数
        char* deleteRecord1(char* p);
        //从内存中读取一个tuple
        Tuple readTuple(const char* p , Attribute attr);
        //获取一个tuple的长度
        int getTupleLength(char* p);
        //判断插入的记录是否和其他记录冲突
        bool isConflict(std::vector<Tuple>& tuples , std::vector<Data>& v , int index);
        //带索引查找
        void searchWithIndex(std::string table_name , std::string target_attr , Where where , std::vector<int>& block_ids);
        //在块中进行条件删除
        int conditionDeleteInBlock(std::string table_name , int block_id , Attribute attr , int index , Where where);
        //在块中进行条件查询
        void conditionSelectInBlock(std::string table_name , int block_id , Attribute attr , int index , Where where , std::vector<Tuple>& v);
};

#endif