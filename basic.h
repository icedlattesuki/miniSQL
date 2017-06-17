//
//  basic.h
//  base
//
//  Created by Sr on 2017/5/25.
//  Copyright © 2017年 Sr. All rights reserved.
//

#ifndef _BASIC_H_
#define _BASIC_H_ 1
#include <iostream>
#include <vector>

//用于where的判断 分别为小于，小于等于，等于，大于等于，大于，不等于
typedef enum{
    LESS,
    LESS_OR_EQUAL,
    EQUAL,
    GREATER_OR_EQUAL,
    GREATER,
    NOT_EQUAL
} WHERE;

//一个struct存放它的一条信息的类型和值
//用一个strunt嵌套一个union实现多种类型的转换
//type的类型：-1：int,0:float,1-255:string(数值为字符串的长度+1),注意使用时对Value的选择！
struct Data{
    int type;
    int datai;
    float dataf;
    std::string datas;
};

//Where存放一组判断语句
struct Where{
    Data data; //数据
    WHERE relation_character;   //关系
};

//在确定类型时，慎用str.size()+1来决定str的type的值，一张表最多32个attribute
struct Attribute{
    int num;  //存放table的属性数
    std::string name[32];  //存放每个属性的名字
    short type[32];  //存放每个属性的类型，-1：int,0:float,1~255:string的长度+1
    bool unique[32];  //判断每个属性是否unique，是为true
    short primary_key;  //判断是否存在主键,-1为不存在，其他则为主键的所在位置
    bool has_index[32]; //判断是否存在索引
};

//索引管理，一张表最多只能有10个index
struct Index{
    int num;  //index的总数
    short location[10];  //每个index在Attribute的name数组中是第几个
    std::string indexname[10];  //每个index的名字
};

//元组管理，数据只增不减
class Tuple{
private:
    std::vector<Data> data_;  //存储元组里的每个数据的信息
    bool isDeleted_;
public:
    Tuple() : isDeleted_(false) {};
    Tuple(const Tuple &tuple_in);  //拷贝元组
    void addData(Data data_in);  //新增元组
    std::vector<Data> getData() const;  //返回数据
    int getSize();  //返回元组的数据数量
    bool isDeleted();
    void setDeleted();
    void showTuple();  //显示元组中的所有数据
};

class Table{
private:
    std::string title_;  //表名
    std::vector<Tuple> tuple_;  //存放所有的元组
    Index index_;  //表的索引信息
public:
    Attribute attr_;  //数据的类型
    //构造函数
    Table(){};
    Table(std::string title,Attribute attr);
    Table(const Table &table_in);

    // int DataSize();  //每个tuple占的数据大小

    int setIndex(short index,std::string index_name);  //插入索引，输入要建立索引的Attribute的编号，以及索引的名字，成功返回1失败返回0
    int dropIndex(std::string index_name);  //删除索引，输入建立的索引的名字，成功返回1失败返回0

    //private的输出接口
    std::string getTitle();
    Attribute getAttr();
    std::vector<Tuple>& getTuple();
    Index getIndex();
    short gethasKey();

    void showTable(); //显示table的部分数据
    void showTable(int limit);
};


#endif
