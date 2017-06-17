#ifndef _INTERPRETER_H_
#define	_INTERPRETER_H_ 1

#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <fstream>
#include "catalog_manager.h"
#include "api.h"
#include "basic.h"

class Interpreter{
public:
    Interpreter();
    
    //功能：获取一行输入的信息，并将输入的格式规范化
    //异常：无异常
	void getQuery();
    //功能：对字符串进行初步解析，判断这个字符串的功能
    //异常：如果出现输入的关键字不存在，则抛出异常input_format_error
    void EXEC();
    //输入：select last_name,first_name,birth,state from president where t1<10 and t2>20 ;
    //输出：Success或者异常
    //功能：进行选择操作，支持单表多约束
    //异常：格式错误则抛出input_format_error
    //如果表不存在，抛出table_not_exist异常
    //如果属性不存在，抛出attribute_not_exist异常
    //如果Where条件中的两个数据类型不匹配，抛出data_type_conflict异常
    void EXEC_SELECT();
    //输入：drop table t1;
    //输出：Success或者异常
    //功能：删除表t1
    //异常：格式错误则抛出input_format_error
    //如果表不存在，抛出table_not_exist异常
    void EXEC_DROP_TABLE();
    //输入：drop index ID_index on t1;
    //输出：Success或者异常
    //功能：在表t1中删除一个名字叫ID_index的索引
    //异常：格式错误则抛出input_format_error异常
    //如果表不存在，抛出table_not_exist异常
    //如果对应属性不存在，抛出attribute_not_exist异常
    //如果对应属性没有索引，抛出index_not_exist异常
    void EXEC_DROP_INDEX();
    //输入：create table T1(
    //            NAME char(32),
    //            ID int unique,
    //            SCORE float,
    //            primary key (ID));
    //输出：Success或者异常
    //功能：在数据库中插入一个表的元信息
    //异常：格式错误则抛出input_format_error异常
    //如果表不存在，抛出table_not_exist异常
    void EXEC_CREATE_TABLE();
    //输入：create index ID_index on t1 (id);
    //输出：Success或者异常
    //功能：在表中插入一个名字叫ID_index的索引，其对应属性为ID
    //异常：格式错误则抛出input_format_error异常
    //如果表不存在，抛出table_not_exist异常
    //如果对应属性不存在，抛出attribute_not_exist异常
    //如果对应属性已经有了索引，抛出index_exist异常
    void EXEC_CREATE_INDEX();
    //输入：insert into T1 values('WuZhaoHui',0001,99.99);
    //输出：Success或者异常
    //功能：向T1内插入值的信息
    //异常：
    void EXEC_INSERT();
    //输入：delete from MyClass where id=1;
    //     delete * from MyClass;
    //where中只存在一条信息
    //输出：Success或者异常
    //功能：从Myclass中删除id=1的元组
    //异常：格式错误则抛出input_format_error异常
    //如果表不存在，抛出table_not_exist异常
    //如果属性不存在，抛出attribute_not_exist异常
    //如果Where条件中的两个数据类型不匹配，抛出data_type_conflict异常
    void EXEC_DELETE();
    //输入：describe T1;
    //     或者 desc T1；
    //功能：输出表T1的所有属性，索引的基本信息
    void EXEC_SHOW();
    //输入：exit;
    //功能：退出数据库
    void EXEC_EXIT();
    //输入：execfile 文件路径
    //功能：根据文件路径读取文件信息，并用于数据库的操作
    void EXEC_FILE();
    
private:
    //字符串规范化函数
    void Normalize();
    //存放输入的字符串和规范化后的字符串
    std::string query;
    //输入：所对应的字符的开头位置，引用传出该字符的结尾位置
    //输出：这个位置所对应的单词的字符串
    //功能：从query中取字
    std::string getWord(int pos,int &end_pos);
    //输入：所需要转成小写的字符串，pos位置为所对应的单词的开始的位置
    //输出：将pos位置的单词改成小写后，输出更改后的完整字符串
    //功能：将字符串str中的pos位置开头的单词转化成小写，用于标准化
    std::string getLower(std::string str,int pos);
    //输入：所对应的字符的开头位置，引用传出该字符的结尾位置
    //输出：这个位置所对应的关系符号
    //功能：从query中取出关系符号
    std::string getRelation(int pos,int &end_pos);
    //输入：所对应的字符的开头位置，引用传出该字符的结尾位置
    //输出：返回一个类型（-1～255）
    int getType(int pos,int &end_pos);
    //输出：返回一个整数的位数
    int getBits(int num);
    //输出：返回一个浮点数的位数（保留小数点后4位）
    int getBits(float num);
};

template <class Type>
Type stringToNum(const std::string& str)
{
    std::istringstream iss(str);
    Type num;
    iss >> num;
    return num;
}

#endif
