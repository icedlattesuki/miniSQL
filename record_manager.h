#ifndef _RECORD_MANAGER_H_
#define _RECORD_MANAGER_H_ 1

#include <cstdio> 
#include <iostream>
#include <string>
#include <vector>
#include "basic.h"
#include "catalog_manager.h"
#include "buffer_manager.h"
#include "exception.h"

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
        //主键冲突，抛出primary_key_conflict异常。如果表不存在，抛出table_not_exist异常。
        void insertRecord(std::string table_name , const Tuple& tuple); 
        //输入：表名
        //输出：void
        //功能：删除对应表中所有记录（不删除表文件）
        //异常：如果表不存在，抛出table_not_exist异常
        void deleteRecord(std::string table_name);
        //输入：表名，目标属性，一个Where类型的对象
        //输出：void
        //功能：删除对应表中所有目标属性值满足Where条件的记录
        //异常：如果表不存在，抛出table_not_exist异常。如果属性不存在，抛出attribute_not_exist异常。
        //如果Where条件中的两个数据类型不匹配，抛出data_type_conflict异常。
        void deleteRecord(std::string table_name , std::string target_attr , Where where);
        //输入：表名
        //输出：Table类型对象
        //功能：返回整张表
        //异常：如果表不存在，抛出table_not_exist异常
        Table selectRecord(std::string table_name);
        //输入：表名，目标属性，一个Where类型的对象
        //输出：Table类型对象
        //功能：返回包含所有目标属性满足Where条件的记录的表
        //异常：如果表不存在，抛出table_not_exist异常。如果属性不存在，抛出attribute_not_exist异常。
        //如果Where条件中的两个数据类型不匹配，抛出data_type_conflict异常。
        Table selectRecord(std::string table_name , std::string target_attr , Where where);
};

#endif