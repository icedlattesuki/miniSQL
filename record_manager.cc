//
//  record_manager.cc
//  record_manager
//
//  Created by Hys on 2017/5/30.
//  Copyright © 2017年 Hys. All rights reserved.
//
#include "record_manager.h"

//输入：表名
//输出：void
//功能：建立表文件
//异常：无异常处理（由catalog manager处理）
void RecordManager::createTableFile(std::string table_name) {
    table_name = "./database/data/" + table_name;
    FILE* f = fopen(table_name.c_str() , "w");
    fclose(f);
}

//输入：表名
//输出：void
//功能：删除表文件
//异常：无异常处理（由catalog manager处理）
void RecordManager::dropTableFile(std::string table_name) {
    table_name = "./database/data/" + table_name;
    remove(table_name.c_str());
}

//输入：表名，一个元组
//输出：void
//功能：向对应表中插入一条记录
//异常：如果元组类型不匹配，抛出tuple_type_conflict异常。如果
//主键冲突，抛出primary_key_conflict异常。如果unique属性冲突，
//抛出unique_conflict异常。如果表不存在，抛出table_not_exist异常。
void RecordManager::insertRecord(std::string table_name , Tuple& tuple) {
    std::string tmp_name = table_name;
    table_name = "./database/data/" + table_name;
    CatalogManager catalog_manager;
    //检测表是否存在
    if (!catalog_manager.hasTable(tmp_name)) {
        throw table_not_exist();
    }
    Attribute attr = catalog_manager.getAttribute(tmp_name);
    std::vector<Data> v = tuple.getData();
    //检测插入的元组的各个属性是否合法
    for (int i = 0;i < v.size();i++) {
        if (v[i].type != attr.type[i])
            throw tuple_type_conflict();
    }
    Table table = selectRecord(tmp_name);
    std::vector<Tuple>& tuples = table.getTuple();
    //检测是否存在主键冲突
    if (attr.primary_key >= 0) {
        if (isConflict(tuples , v , attr.primary_key) == true) 
            throw primary_key_conflict();
    }
    //检测是否存在unqiue冲突
    for (int i = 0;i < attr.num;i++) {
        if (attr.unique[i] == true) {
            if (isConflict(tuples , v , i) == true)
                throw unique_conflict();
        }
    }

    //异常检测完成

    //获取表所占的块的数量
    // int block_num = getFileSize(table_name) / PAGESIZE;
    // 改为
    int block_num = getBlockNum(table_name);
    //处理表文件大小为0的特殊情况
    if (block_num <= 0)
        block_num = 1;
    //获取表的最后一块的句柄
    char* p = buffer_manager.getPage(table_name , block_num - 1);
    int i;
    //寻找第一个空位
    for (i = 0;p[i] != '\0' && i < PAGESIZE;i++) ;
    int j;
    int len = 0;
    //计算插入的tuple的长度
    for (j = 0;j < v.size();j++) {
        Data d = v[j];
        switch(d.type) {
            case -1:{
                int t = getDataLength(d.datai);
                len += t;
            };break;
            case 0:{
                float t = getDataLength(d.dataf);
                len += t;
            };break;
            default:{
                len += d.datas.length();
            };
        }
    }
    len += v.size() + 7;
    int block_offset;//最终记录所插入的块的编号
    //如果剩余的空间足够插入该tuple
    if (PAGESIZE - i >= len) {
        block_offset = block_num - 1;
        //插入该元组
        insertRecord1(p , i , len , v);
        //写回表文件
        int page_id = buffer_manager.getPageId(table_name , block_num - 1);
        // buffer_manager.flushPage(page_id , table_name , block_num - 1);
        // 改为
        buffer_manager.modifyPage(page_id);
    }
    //如果剩余的空间不够
    else {
        block_offset = block_num;
        //新增一个块
        char* p = buffer_manager.getPage(table_name , block_num);
        //在新增的块中插入该元组
        insertRecord1(p , 0 , len , v);
        //写回表文件
        int page_id = buffer_manager.getPageId(table_name , block_num);
        // buffer_manager.flushPage(page_id , table_name , block_num);
        // 改为
        buffer_manager.modifyPage(page_id);
    }

    //更新索引
    IndexManager index_manager(tmp_name);
    for (int i = 0;i < attr.num;i++) {
        if (attr.has_index[i] == true) {
            std::string attr_name = attr.name[i];
            std::string file_path = "INDEX_FILE_" + attr_name + "_" + tmp_name;
            std::vector<Data> d = tuple.getData();
            index_manager.insertIndex(file_path , d[i] , block_offset);
        }
    }
}

// 输入：表名
// 输出：int(删除的记录数)
// 功能：删除对应表中所有记录（不删除表文件）
// 异常：如果表不存在，抛出table_not_exist异常
int RecordManager::deleteRecord(std::string table_name) {
    std::string tmp_name = table_name;
    table_name = "./database/data/" + table_name;
    CatalogManager catalog_manager;
    //检测表是否存在
    if (!catalog_manager.hasTable(tmp_name)) {
        throw table_not_exist();
    }
    //获取文件所占块的数量
    // int block_num = getFileSize(table_name) / PAGESIZE;
    // 改为
    int block_num = getBlockNum(table_name);
    //表文件大小为0时直接返回
    if (block_num <= 0)
        return 0;
    Attribute attr = catalog_manager.getAttribute(tmp_name);
    IndexManager index_manager(tmp_name);
    int count = 0;
    //遍历所有块
    for (int i = 0;i < block_num;i++) {
        //获取当前块的句柄
        char* p = buffer_manager.getPage(table_name , i);
        char* t = p;
        //将块中的每一个元组记录设置为已删除
        while(*p != '\0' && p < t + PAGESIZE) {
            //更新索引
            Tuple tuple = readTuple(p , attr);
            for (int j = 0;j < attr.num;j++) {
                if (attr.has_index[j] == true) {
                    std::string attr_name = attr.name[i];
                    std::string file_path = "INDEX_FILE_" + attr_name + "_" + tmp_name;
                    std::vector<Data> d = tuple.getData();
                    index_manager.deleteIndexByKey(file_path , d[j]);
                }
            }
            //删除记录
            p = deleteRecord1(p);
            count++;
        }
        //将块写回表文件
        int page_id = buffer_manager.getPageId(table_name , i);
        // buffer_manager.flushPage(page_id , table_name , i);
        // 改为
        buffer_manager.modifyPage(page_id);
    }
    return count;
}

//输入：表名，目标属性，一个Where类型的对象
//输出：int(删除的记录数)
//功能：删除对应表中所有目标属性值满足Where条件的记录
//异常：如果表不存在，抛出table_not_exist异常。如果属性不存在，抛出attribute_not_exist异常。
//如果Where条件中的两个数据类型不匹配，抛出data_type_conflict异常。
int RecordManager::deleteRecord(std::string table_name , std::string target_attr , Where where) {
    std::string tmp_name = table_name;
    table_name = "./database/data/" + table_name;
    CatalogManager catalog_manager;     
    //检测表是否存在
    if (!catalog_manager.hasTable(tmp_name)) {
        throw table_not_exist();
    }
    Attribute attr = catalog_manager.getAttribute(tmp_name);
    int index = -1;
    bool flag = false;
    //获取目标属性对应的编号
    for (int i = 0;i < attr.num;i++) {
        if (attr.name[i] == target_attr) {
            index = i;
            if (attr.has_index[i] == true) 
                flag = true;
            break;
        }
    }
    //目标属性不存在，抛出异常
    if (index == -1) {
        throw attribute_not_exist();
    }
    //where条件中的两个数据的类型不匹配，抛出异常
    else if (attr.type[index] != where.data.type) {
        throw data_type_conflict();
    }

    //异常处理完成

    int count = 0;
    //如果目标属性上有索引
    if (flag == true && where.relation_character != NOT_EQUAL) {
        std::vector<int> block_ids;
        //通过索引获取满足条件的记录所在的块号
        searchWithIndex(tmp_name , target_attr , where , block_ids);
        for (int i = 0;i < block_ids.size();i++) {
            count += conditionDeleteInBlock(tmp_name , block_ids[i] , attr , index , where);
        }
    }
    else {
        //获取文件所占块的数量
        // int block_num = getFileSize(table_name) / PAGESIZE;
        // 改为
        int block_num = getBlockNum(table_name);
        //文件大小为0，直接返回
        if (block_num <= 0)
            return 0;
        //遍历所有的块
        for (int i = 0;i < block_num;i++) {
            count += conditionDeleteInBlock(tmp_name , i , attr , index , where);
        }
    }
    return count;
}

//输入：表名
//输出：Table类型对象
//功能：返回整张表
//异常：如果表不存在，抛出table_not_exist异常
Table RecordManager::selectRecord(std::string table_name , std::string result_table_name) {
    std::string tmp_name = table_name;
    table_name = "./database/data/" + table_name;
    CatalogManager catalog_manager;    
    //检测表是否存在
    if (!catalog_manager.hasTable(tmp_name)) {
        throw table_not_exist();
    }
    //获取文件所占的块的数量
    // int block_num = getFileSize(table_name) / PAGESIZE;
    // 改为
    int block_num = getBlockNum(table_name);
    //处理文件大小为0的特殊情况
    if (block_num <= 0)
        block_num = 1;
    //获取表的属性
    Attribute attr = catalog_manager.getAttribute(tmp_name);
    //构建table类的实例
    Table table(result_table_name , attr);
    std::vector<Tuple>& v = table.getTuple();
    //遍历所有块
    for (int i = 0;i < block_num;i++) {
        //获取当前块的句柄
        char* p = buffer_manager.getPage(table_name , i);
        char* t = p;
        //遍历块中所有记录
        while (*p != '\0' && p < t + PAGESIZE) {
            //读取记录
            Tuple tuple = readTuple(p , attr);
            //如果记录没有被删除，将其添加到table中
            if (tuple.isDeleted() == false)
                v.push_back(tuple);
            int len = getTupleLength(p);
            p = p + len;
        }
    }
    return table;
}

//输入：表名，目标属性，一个Where类型的对象
//输出：Table类型对象
//功能：返回包含所有目标属性满足Where条件的记录的表
//异常：如果表不存在，抛出table_not_exist异常。如果属性不存在，抛出attribute_not_exist异常。
//如果Where条件中的两个数据类型不匹配，抛出data_type_conflict异常。
Table RecordManager::selectRecord(std::string table_name , std::string target_attr , Where where , std::string result_table_name) {
    std::string tmp_name = table_name;
    table_name = "./database/data/" + table_name;
    CatalogManager catalog_manager;
    //检测表是否存在
    if (!catalog_manager.hasTable(tmp_name)) {
        throw table_not_exist();
    }
    Attribute attr = catalog_manager.getAttribute(tmp_name);
    int index = -1; 
    bool flag = false;
    //获取目标属性的编号
    for (int i = 0;i < attr.num;i++) {
        if (attr.name[i] == target_attr) {
            index = i;
            if (attr.has_index[i] == true)
                flag = true;
            break;
        }
    }
    //目标属性不存在，抛出异常
    if (index == -1) {
        throw attribute_not_exist();
    }
    //where条件中的两个数据的类型不匹配，抛出异常
    else if (attr.type[index] != where.data.type) {
        throw data_type_conflict();
    }

    //异常检测完成

    //构建table
    Table table(result_table_name , attr);
    std::vector<Tuple>& v = table.getTuple();
    if (flag == true && where.relation_character != NOT_EQUAL) {
        std::vector<int> block_ids;
        //使用索引获取满足条件的记录所在块号
        searchWithIndex(tmp_name , target_attr , where , block_ids);
        for (int i = 0; i < block_ids.size();i++) {
            conditionSelectInBlock(tmp_name , block_ids[i] , attr , index , where , v);
        }
    }
    else {
        //获取文件所占的块的数量
        // int block_num = getFileSize(table_name) / PAGESIZE;
        // 改为
        int block_num = getBlockNum(table_name);
        //处理文件大小为0的特殊情况
        if (block_num <= 0)
            block_num = 1;
        //遍历所有块
        for (int i = 0;i < block_num;i++) {
            conditionSelectInBlock(tmp_name , i , attr , index , where , v);
        }
    }
    return table;
}

//输入：表名，目标属性名
//输出：void
//功能：对表中已经存在的记录建立索引
//异常：如果表不存在，抛出table_not_exist异常。如果属性不存在，抛出attribute_not_exist异常。
void RecordManager::createIndex(IndexManager& index_manager , std::string table_name , std::string target_attr) {
    std::string tmp_name = table_name;
    table_name = "./database/data/" + table_name;
    CatalogManager catalog_manager;
    //检测表是否存在
    if (!catalog_manager.hasTable(tmp_name)) {
        throw table_not_exist();
    }
    Attribute attr = catalog_manager.getAttribute(tmp_name);
    int index = -1;
    //获取目标属性的编号
    for (int i = 0;i < attr.num;i++) {
        if (attr.name[i] == target_attr) {
            index = i;
            break;
        }
    }
    //目标属性不存在，抛出异常
    if (index == -1) {
        throw attribute_not_exist();
    }
    //异常检测完成

    //获取文件所占的块的数量
    // int block_num = getFileSize(table_name) / PAGESIZE;
    // 改为
    int block_num = getBlockNum(table_name);
    //处理文件大小为0的特殊情况
    if (block_num <= 0)
        block_num = 1;
    //获取表的属性
    std::string file_path = "INDEX_FILE_" + target_attr + "_" + tmp_name;
    //遍历所有块
    for (int i = 0;i < block_num;i++) {
        //获取当前块的句柄
        char* p = buffer_manager.getPage(table_name , i);
        char* t = p;
        //遍历块中所有记录
        while (*p != '\0' && p < t + PAGESIZE) {
            //读取记录
            Tuple tuple = readTuple(p , attr);
            if (tuple.isDeleted() == false) {
                std::vector<Data> v = tuple.getData();
                index_manager.insertIndex(file_path , v[index] , i);
            }
            int len = getTupleLength(p);
            p = p + len;
        }
    }
}


//以下是几个辅助函数，不详细注释了

//获取文件大小
int RecordManager::getBlockNum(std::string table_name) {
    char* p;
    int block_num = -1;
    do {
        p = buffer_manager.getPage(table_name , block_num + 1);
        block_num++;
    } while(p[0] != '\0');
    return block_num;
}

//insertRecord的辅助函数
void RecordManager::insertRecord1(char* p , int offset , int len , const std::vector<Data>& v) {
    std::stringstream stream;
    stream << len;
    std::string s = stream.str();
    while (s.length() < 4) 
        s = "0" + s;
    for (int j = 0;j < s.length();j++,offset++)
        p[offset] = s[j];
    for (int j = 0;j < v.size();j++) {
        p[offset] = ' ';
        offset++;
        Data d = v[j];
        switch(d.type) {
            case -1:{
                copyString(p , offset , d.datai);
            };break;
            case 0:{
                copyString(p , offset , d.dataf);
            };break;
            default:{
                copyString(p , offset , d.datas);
            };
        }
    }
    p[offset] = ' ';
    p[offset + 1] = '0';
    p[offset + 2] = '\n';
}

//deleteRecord的辅助函数
char* RecordManager::deleteRecord1(char* p) {
    int len = getTupleLength(p);
    p = p + len;
    *(p - 2) = '1';
    return p;
}

//从内存中读取一个tuple
Tuple RecordManager::readTuple(const char* p , Attribute attr) {
    Tuple tuple;
    p = p + 5;
    for (int i = 0;i < attr.num;i++) {
        Data data;
        data.type = attr.type[i];
        char tmp[100];
        int j;
        for (j = 0;*p != ' ';j++,p++) {
            tmp[j] = *p;
        }
        tmp[j] = '\0';
        p++;
        std::string s(tmp);
        switch(data.type) {
            case -1:{
                std::stringstream stream(s);
                stream >> data.datai;
            };break;
            case 0:{
                std::stringstream stream(s);
                stream >> data.dataf;
            };break;
            default:{
                data.datas = s;
            }
        }
        tuple.addData(data);
    }
    if (*p == '1')
        tuple.setDeleted();
    return tuple;
}

//获取一个tuple的长度
int RecordManager::getTupleLength(char* p) {
    char tmp[10];
    int i;
    for (i = 0;p[i] != ' ';i++) 
        tmp[i] = p[i];
    tmp[i] = '\0';
    std::string s(tmp);
    int len = stoi(s);
    return len;
}

//判断插入的记录是否和其他记录冲突
bool RecordManager::isConflict(std::vector<Tuple>& tuples , std::vector<Data>& v , int index) {
    for (int i = 0;i < tuples.size();i++) {
        if (tuples[i].isDeleted() == true)
            continue;
        std::vector<Data> d = tuples[i].getData();
        switch(v[index].type) {
            case -1:{
                if (v[index].datai == d[index].datai)
                    return true;
            };break;
            case 0:{
                if (v[index].dataf == d[index].dataf)
                    return true;
            };break;
            default:{
                if (v[index].datas == d[index].datas)
                    return true;
            };
        }
    }
    return false;
}

//带索引查找
void RecordManager::searchWithIndex(std::string table_name , std::string target_attr , Where where , std::vector<int>& block_ids) {
    IndexManager index_manager(table_name);
    Data tmp_data;
    std::string file_path = "INDEX_FILE_" + target_attr + "_" + table_name;
    if (where.relation_character == LESS || where.relation_character == LESS_OR_EQUAL) {
        if (where.data.type == -1) {
            tmp_data.type = -1;
            tmp_data.datai = -INF;
        }
        else if (where.data.type == 0) {
            tmp_data.type = 0;
            tmp_data.dataf = -INF;
        }
        else {
            tmp_data.type = 1;
            tmp_data.datas = "";
        }
        index_manager.searchRange(file_path , tmp_data , where.data , block_ids);
    }
    else if (where.relation_character == GREATER || where.relation_character == GREATER_OR_EQUAL) {
        if (where.data.type == -1) {
            tmp_data.type = -1;
            tmp_data.datai = INF;
        }
        else if (where.data.type == 0) {
            tmp_data.type = 0;
            tmp_data.dataf = INF;
        }
        else {
            tmp_data.type = -2;
        }
        index_manager.searchRange(file_path , where.data , tmp_data , block_ids);
    }
    else {
        index_manager.searchRange(file_path , where.data , where.data , block_ids);
    }
}

//在块中进行条件删除
int RecordManager::conditionDeleteInBlock(std::string table_name , int block_id , Attribute attr , int index , Where where) {
    //获取当前块的句柄
    table_name = "./database/data/" + table_name;//新增
    char* p = buffer_manager.getPage(table_name , block_id);
    char* t = p;
    int count = 0;
    //遍历块中所有记录
    while (*p != '\0' && p < t + PAGESIZE) {
        //读取记录
        Tuple tuple = readTuple(p , attr);
        std::vector<Data> d = tuple.getData();
        //根据属性类型执行不同操作
        switch(attr.type[index]) {
            case -1:{
                //如果满足where条件
                if (isSatisfied(d[index].datai , where.data.datai , where.relation_character) == true) {
                    //将记录删除
                    p = deleteRecord1(p);
                    count++;
                }
                //如果不满足where条件，跳过该记录
                else {
                    int len = getTupleLength(p);
                    p = p + len;
                }
            };break;
            //同case1
            case 0:{
                if (isSatisfied(d[index].dataf , where.data.dataf , where.relation_character) == true) {
                    p = deleteRecord1(p);
                    count++;
                }
                else {
                    int len = getTupleLength(p);
                    p = p + len;
                }
            };break;
            //同case1
            default:{
                if (isSatisfied(d[index].datas , where.data.datas , where.relation_character) == true) {
                    p = deleteRecord1(p);
                    count++;
                }
                else {
                    int len = getTupleLength(p);
                    p = p + len;
                }
            }
        }
    }
    //将当前块写回文件
    int page_id = buffer_manager.getPageId(table_name , block_id);
    // buffer_manager.flushPage(page_id , table_name , block_id);
    // 改为
    buffer_manager.modifyPage(page_id);
    return count;
}

//在块中进行条件查询
void RecordManager::conditionSelectInBlock(std::string table_name , int block_id , Attribute attr , int index , Where where , std::vector<Tuple>& v) {
    //获取当前块的句柄
    table_name = "./database/data/" + table_name;//新增
    char* p = buffer_manager.getPage(table_name , block_id);
    char* t = p;
    //遍历所有记录
    while (*p != '\0' && p < t + PAGESIZE) {
        //读取记录
        Tuple tuple = readTuple(p , attr);
        //如果记录已被删除，跳过该记录
        if (tuple.isDeleted() == true) {
            int len = getTupleLength(p);
            p = p + len;
            continue;
        }
        std::vector<Data> d = tuple.getData();
        //根据属性类型选择
        switch(attr.type[index]) {
            case -1:{
                //满足条件，则将该元组添加到table
                if (isSatisfied(d[index].datai , where.data.datai , where.relation_character) == true) {
                    v.push_back(tuple);
                }
                //不满足条件，跳过该记录
            };break;
            //同case1
            case 0:{
                if (isSatisfied(d[index].dataf , where.data.dataf , where.relation_character) == true) {
                    v.push_back(tuple);
                }
            };break;
            //同case1
            default:{
                if (isSatisfied(d[index].datas , where.data.datas , where.relation_character) == true) {
                    v.push_back(tuple);
                }
            };
        }
        int len = getTupleLength(p);
        p = p + len;
    }
}
