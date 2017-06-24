//
//  catalog.cpp
//  catalog
//
//  Created by Sr on 2017/5/24.
//  Copyright © 2017年 Sr. All rights reserved.
//

#include "catalog_manager.h"

void CatalogManager::createTable(std::string name, Attribute Attr, int primary, Index index){
    //如果已存在该表，则异常
    if(hasTable(name)){
        throw table_exist();
    }
    //确保主键为unique
    Attr.unique[primary]=true;
    //记录每条信息的字符数（包括这里的5个）
    std::string str_tmp="0000 ";
    //添加name
    str_tmp+=name;
    //添加attribute的数量
    str_tmp=str_tmp+" "+num2str(Attr.num, 2);
    //添加没个attribute的信息，顺序为类型，名字，是否为唯一
    for(int i=0;i<Attr.num;i++)
        str_tmp=str_tmp+" "+num2str(Attr.type[i], 3)+" "+Attr.name[i]+" "+(Attr.unique[i]==true?"1":"0");
    //添加主键信息
    str_tmp=str_tmp+" "+num2str(primary, 2);
    //添加index的数量, ;用来做标记index的开始
    str_tmp=str_tmp+" ;"+num2str(index.num, 2);
    //添加index的信息，顺序为相对位置和名字
    for(int i=0;i<index.num;i++)
        str_tmp=str_tmp+" "+num2str(index.location[i], 2)+" "+index.indexname[i];
    //换行后在结尾接上一个#，每个块以#结尾
    str_tmp=str_tmp+"\n"+"#";
    //更改每条信息的长度的记录
    std::string str_len=num2str((int)str_tmp.length()-1, 4);
    str_tmp=str_len+str_tmp.substr(4,str_tmp.length()-4);
    //计算块的数量
    int block_num=getBlockNum(TABLE_MANAGER_PATH)/PAGESIZE;
    //处理当块的数量为0的特殊情况
    if(block_num<=0)
        block_num=1;
    //遍历所有的块
    for(int current_block=0;current_block<block_num;current_block++){
        char* buffer = buffer_manager.getPage(TABLE_MANAGER_PATH , current_block);
        int page_id = buffer_manager.getPageId(TABLE_MANAGER_PATH , current_block);
        //寻找该block的有效长度
        int length=0;
        for(length=0;length<PAGESIZE&&buffer[length]!='\0'&&buffer[length]!='#';length++){}
        //确保插入新信息后该页长度不会超过PAGESIZE
        if(length+(int)str_tmp.length()<PAGESIZE){
            //删除#
            if(length&&buffer[length-1]=='#')
                buffer[length-1]='\0';
            else if(buffer[length]=='#')
                buffer[length]='\0';
            //字符串拼接
            strcat(buffer , str_tmp.c_str());
            //保存并刷新该页后返回
            buffer_manager.modifyPage(page_id);
            return;
        }
    }
    //如果之前的块不够用，就新建一块后直接把信息插入
    char* buffer = buffer_manager.getPage(TABLE_MANAGER_PATH , block_num);
    int page_id = buffer_manager.getPageId(TABLE_MANAGER_PATH , block_num);
    strcat(buffer , str_tmp.c_str());
    buffer_manager.modifyPage(page_id);
    
}

void CatalogManager::dropTable(std::string name){
    //如果不存在该表，则异常
    if(!hasTable(name)){
        throw table_not_exist();
    }
    //寻找table_name所对应的块号和在该块的位置
    int suitable_block;
    int start_index=getTablePlace(name,suitable_block);
    //得到所对应块的信息
    char* buffer = buffer_manager.getPage(TABLE_MANAGER_PATH , suitable_block);
    int page_id = buffer_manager.getPageId(TABLE_MANAGER_PATH , suitable_block);
    std::string buffer_check(buffer);
    //求出应删除的块的index的开始和结尾后删除
    int end_index=start_index+str2num(buffer_check.substr(start_index,4));
    int index=0,current_index=0;;
    do{
        if(index<start_index||index>=end_index)
            buffer[current_index++]=buffer[index];
        index++;
    }while(buffer[index]!='#');
    buffer[current_index++]='#';
    buffer[current_index]='\0';
    //刷新页面
    buffer_manager.modifyPage(page_id);
}

Attribute CatalogManager::getAttribute(std::string name){
    //如果不存在该表，则异常
    if(!hasTable(name)){
        throw attribute_not_exist();
    }
    //寻找table_name对应的块和在块中的位置
    int suitable_block;
    int start_index=getTablePlace(name,suitable_block);
    //得到所对应的块的信息
    char* buffer = buffer_manager.getPage(TABLE_MANAGER_PATH , suitable_block);
    std::string buffer_check(buffer);
    //end_index记录该行中表的名字的最后一个字符的位置
    int end_index=0;
    std::string table_name=getTableName(buffer_check, start_index, end_index);
    Attribute table_attr;
    start_index=end_index+1;
    //得到attribute数量的字符串
    std::string attr_num=buffer_check.substr(start_index,2);
    table_attr.num=str2num(attr_num);
    start_index+=3;
    for(int index=0;index<table_attr.num;index++){
        //对所有的attribute的类型和名字
        if(buffer_check[start_index]=='-'){
            table_attr.type[index]=-1;
            start_index+=5;
            while(buffer_check[start_index]!=' '){
                table_attr.name[index]+=buffer_check[start_index++];
            }
            start_index+=1;
            table_attr.unique[index]=(buffer_check[start_index]=='1'?true:false);
        }
        else if(str2num(buffer_check.substr(start_index,3))==0){
            table_attr.type[index]=0;
            start_index+=4;
            while(buffer_check[start_index]!=' '){
                table_attr.name[index]+=buffer_check[start_index++];
            }
            start_index+=1;
            table_attr.unique[index]=(buffer_check[start_index]=='1'?true:false);
        }
        else{
            table_attr.type[index]=str2num(buffer_check.substr(start_index,3));
            start_index+=4;
            while(buffer_check[start_index]!=' '){
                table_attr.name[index]+=buffer_check[start_index++];
            }
            start_index+=1;
            table_attr.unique[index]=(buffer_check[start_index]=='1'?true:false);
        }
        start_index+=2;
    }
    //记录primary_key的信息
    if(buffer_check[start_index]=='-')
        table_attr.primary_key=-1;
    else
        table_attr.primary_key=str2num(buffer_check.substr(start_index,2));
    //设置index的信息
    Index index_record=getIndex(table_name);
    for(int i=0;i<32;i++)
        table_attr.has_index[i]=false;
    for(int i=0;i<index_record.num;i++)
        table_attr.has_index[index_record.location[i]]=true;
    
    return table_attr;
}

//判断在所有的attribute中是否存在属性的名字
bool CatalogManager::hasAttribute(std::string table_name , std::string attr_name){
    //如果不存在该表，则异常
    if(!hasTable(table_name)){
        throw table_not_exist();
    }
    //得到attribute的信息后，遍历所有的名字
    Attribute find_attr=getAttribute(table_name);
    for(int index=0;index<find_attr.num;index++){
        if(attr_name==find_attr.name[index])
            return true;
    }
    return false;
}

//根据index_name返回attr_name
std::string CatalogManager::IndextoAttr(std::string table_name, std::string index_name){
    if(!hasTable(table_name))
        throw table_not_exist();
    Index index_record=getIndex(table_name);
    int hasfind=-1;
    for(int i=0;i<index_record.num;i++){
        if(index_record.indexname[i]==index_name){
            hasfind=i;
            break;
        }
    }
    if(hasfind==-1)
        throw index_not_exist();
    Attribute attr_record=getAttribute(table_name);
    return attr_record.name[index_record.location[hasfind]];
}

void CatalogManager::createIndex(std::string table_name,std::string attr_name,std::string index_name){
    //如果不存在该表，则异常
    if(!hasTable(table_name))
        throw table_not_exist();
    //如果不存在该属性，则异常
    if(!hasAttribute(table_name, attr_name))
        throw attribute_not_exist();
    //得到该表的index的信息
    Index index_record=getIndex(table_name);
    //如果index的数量已经大于等于10，则异常
    if(index_record.num>=10)
        throw index_full();
    //得到属性的信息
    Attribute find_attr=getAttribute(table_name);
    //遍历所有已有的index
    for(int i=0;i<index_record.num;i++){
        //索引名重复错误
        if(index_record.indexname[i]==index_name)
            throw index_exist();
        //索引重复
        if(find_attr.name[index_record.location[i]]==attr_name)
            throw index_exist();
    }
    //添加索引
    //添加索引的名字
    index_record.indexname[index_record.num]=index_name;
    //添加索引在attribute的位置
    for(int index=0;index<find_attr.num;index++){
        if(attr_name==find_attr.name[index])
        {
            index_record.location[index_record.num]=index;
            break;
        }
    }
    //总数量加一
    index_record.num++;
    //在原有表中删除该表后再插入，实现刷新
    dropTable(table_name);
    createTable(table_name, find_attr, find_attr.primary_key, index_record);
}

void CatalogManager::dropIndex(std::string table_name,std::string index_name){
    //如果不存在该索引，则异常
    if(!hasTable(table_name)){
        throw table_not_exist();
    }
    //得到该表的index的信息
    Index index_record=getIndex(table_name);
    //得到属性的信息
    Attribute attr_record=getAttribute(table_name);
    //遍历所有的index，查找是否有对应名字的索引，如果不存在则异常
    int hasindex=-1;
    for(int index=0;index<index_record.num;index++){
        if(index_record.indexname[index]==index_name){
            hasindex=index;
            break;
        }
    }
    if(hasindex==-1){
        throw index_not_exist();
    }
    //通过将该信息与最后位置的索引替换的方式来删除索引
    index_record.indexname[hasindex]=index_record.indexname[index_record.num-1];
    index_record.location[hasindex]=index_record.location[index_record.num-1];
    index_record.num--;
    //在原有表中删除该表后再插入，实现刷新
    dropTable(table_name);
    createTable(table_name, attr_record, attr_record.primary_key, index_record);
    
}

void CatalogManager::showTable(std::string table_name){
    //如果不存在该表，则异常
    if(!hasTable(table_name)){
        throw table_not_exist();
    }
    //打印表的名字
    std::cout<<"Table name:"<<table_name<<std::endl;
    Attribute attr_record=getAttribute(table_name);
    Index index_record=getIndex(table_name);
    //寻找最长的index_name的信息，在之后的打印表格中会用到
    int longest=-1;
    for(int index=0;index<attr_record.num;index++){
        if((int)attr_record.name[index].length()>longest)
            longest=(int)attr_record.name[index].length();
    }
    //打印属性
    std::string type;
    std::cout<<"Attribute:"<<std::endl;
    std::cout<<"Num|"<<"Name"<<std::setw(longest+2)<<"|Type"<<type<<std::setw(6)<<"|"<<"Unique|Primary Key"<<std::endl;
    for(int index_out=0;index_out<longest+35;index_out++)
        std::cout<<"-";
    std::cout<<std::endl;
    for(int index=0;index<attr_record.num;index++){
        switch (attr_record.type[index]) {
            case -1:
                type="int";
                break;
            case 0:
                type="float";
                break;
            default:
                type="char("+num2str(attr_record.type[index]-1, 3)+")";
                break;
        }
        std::cout<<index<<std::setw(3-index/10)<<"|"<<attr_record.name[index]<<std::setw(longest-(int)attr_record.name[index].length()+2)<<"|"<<type<<std::setw(10-(int)type.length())<<"|";
        if(attr_record.unique[index])
            std::cout<<"unique"<<"|";
        else
            std::cout<<std::setw(7)<<"|";
        if(attr_record.primary_key==index)
            std::cout<<"primary key";
        std::cout<<std::endl;
    }
    
    for(int index_out=0;index_out<longest+35;index_out++)
        std::cout<<"-";
    
    std::cout<<std::endl;
    
    //打印索引
    std::cout<<"Index:"<<std::endl;
    std::cout<<"Num|Location|Name"<<std::endl;
    longest=-1;
    for(int index_out=0;index_out<index_record.num;index_out++){
        if((int)index_record.indexname[index_out].length()>longest)
            longest=(int)index_record.indexname[index_out].length();
    }
    for(int index_out=0;index_out<((longest+14)>18?(longest+14):18);index_out++)
        std::cout<<"-";
    std::cout<<std::endl;
    for(int index_out=0;index_out<index_record.num;index_out++){
        std::cout<<index_out<<std::setw(3-index_out/10)<<"|"<<index_record.location[index_out]<<std::setw(8-index_record.location[index_out]/10)<<"|"<<index_record.indexname[index_out]<<std::endl;
    }
    for(int index_out=0;index_out<((longest+14)>18?(longest+14):18);index_out++)
        std::cout<<"-";
    std::cout<<std::endl<<std::endl;
}

//判断是否已有重名的表格
bool CatalogManager::hasTable(std::string table_name){
    //计算块的数量
    int block_num=getBlockNum(TABLE_MANAGER_PATH)/PAGESIZE;
    if(block_num<=0)
        block_num=1;
    //遍历所有的块
    for(int current_block=0;current_block<block_num;current_block++){
        char* buffer = buffer_manager.getPage(TABLE_MANAGER_PATH , current_block);
        std::string buffer_check(buffer);
        std::string str_tmp="";
        int start_index=0,end_index=0;
        do{
            //如果一开始就是#，则检查下一块
            if(buffer_check[0]=='#')
                break;
            //得到table的名字，如果与输入的名字相同，则return true
            else if(getTableName(buffer, start_index, end_index)==table_name){
                return true;
            }
            else{
                //通过字符串长度来重新确定下一个table的位置
                start_index+=str2num(buffer_check.substr(start_index,4));
                //排除空文档的特殊条件
                if(!start_index)
                    break;
            }
        }while(buffer_check[start_index]!='#');  //判断是否到头
    }
    return false;
}

//数字转字符串，bit为数字的位数
std::string CatalogManager::num2str(int num,short bit){
    std::string str="";
    if(num<0){
        num=-num;
        str+="-";
    }
    int divisor=pow(10,bit-1);
    for(int i=0;i<bit;i++){
        str+=(num/divisor%10+'0');
        divisor/=10;
    }
    return str;
}

//字符串转数字
int CatalogManager::str2num(std::string str){
    return atoi(str.c_str());
}

//得到该行的表的名字
std::string CatalogManager::getTableName(std::string buffer,int start,int &rear){
    std::string str_tmp="";
    rear=0;
    if(buffer=="")
        return buffer;
    while(buffer[start+rear+5]!=' '){
        rear++;
    }
    str_tmp=buffer.substr(start+5,rear);
    rear=start+5+rear;
    return str_tmp;
}

//得到该表的位置，引用传出该表的所在的块的位置，返回在块中的位置，如果未找到，则返回-1
int CatalogManager::getTablePlace(std::string name,int &suitable_block){
    int block_num=getBlockNum(TABLE_MANAGER_PATH);
    if(block_num<=0)
        block_num=1;
    //遍历所有的块
    for(suitable_block=0;suitable_block<block_num;suitable_block++){
        char* buffer = buffer_manager.getPage(TABLE_MANAGER_PATH , suitable_block);
        std::string buffer_check(buffer);
        std::string str_tmp="";
        int start=0,rear=0;
        do{
            //如果一开始就是#，则检查下一块
            if(buffer_check[0]=='#')
                break;
            if(getTableName(buffer, start, rear)==name){
                return start;
            }
            else{
                //通过字符串长度来重新确定start
                start+=str2num(buffer_check.substr(start,4));
                if(!start)
                    break;
            }
        }while(buffer_check[start]!='#');  //判断是否到头
    }
    return -1;
}

Index CatalogManager::getIndex(std::string table_name){
    Index index_record;
    //得到该表的位置和对应的块
    int suitable_block;
    int start_index=getTablePlace(table_name,suitable_block);
    char* buffer = buffer_manager.getPage(TABLE_MANAGER_PATH , suitable_block);
    //将start_index对齐索引信息的位置
    std::string buffer_check(buffer);
    while(buffer_check[start_index]!=';')
        start_index++;
    start_index++;
    //得到索引的数量
    index_record.num=str2num(buffer_check.substr(start_index,2));
    //得到索引的所有信息
    for(int times=0;times<index_record.num;times++){
        start_index+=3;
        index_record.location[times]=str2num(buffer_check.substr(start_index,2));
        start_index+=3;
        while(buffer_check[start_index]!=' '&&buffer_check[start_index]!='#'&&buffer_check[start_index]!='\n'){
            index_record.indexname[times]+=buffer_check[start_index++];
        }
        start_index-=2;
    }
    return index_record;
}

//获取文件大小
int CatalogManager::getBlockNum(std::string table_name) {
    char* p;
    int block_num = -1;
    do {
        p = buffer_manager.getPage(table_name , block_num + 1);
        block_num++;
    } while(p[0] != '\0');
    return block_num;
}
