//
//  main.cpp
//  base
//
//  Created by Sr on 2017/5/25.
//  Copyright © 2017年 Sr. All rights reserved.
//

#include <iostream>
#include "../basic.h"
using namespace std;

int main(int argc, const char * argv[]) {
    
    //测试：初始化Data
    Data d1,d2,d3;
    string s1="hahaha";
    d1.type=-1;
    d1.values.datai=3;
    d2.type=0;
    d2.values.dataf=3.4;
    d3.type=(int)s1.size()+1;
    d3.values.datas=&s1;
    
    Data d4,d5,d6;
    string s2="yingyingying";
    d4.type=0;
    d4.values.dataf=3;
    d5.type=0;
    d5.values.dataf=3.4;
    d6.type=(int)s2.size()+1;
    d6.values.datas=&s2;
    
    Data d7,d8,d9;
    string s3="pluso";
    d7.type=-1;
    d7.values.datai=4;
    d8.type=0;
    d8.values.dataf=9.1;
    d9.type=(int)s3.size()+1;
    d9.values.datas=&s3;
    
    cout<<endl;
    
    //测试：tuple的初始化
    class Tuple T1,T2,T3;
    T1.addData(d1);
    T1.addData(d2);
    T1.addData(d3);
    cout<<"the size of T1 is "<<T1.getSize()<<endl;
    T1.showTuple();
    
    T2.addData(d4);
    T2.addData(d5);
    T2.addData(d6);
    cout<<"the size of T2 is "<<T2.getSize()<<endl;
    T2.showTuple();
    
    T3.addData(d7);
    T3.addData(d8);
    T3.addData(d9);
    cout<<"the size of T3 is "<<T3.getSize()<<endl;
    T3.showTuple();
    
    cout<<endl;
    
    //测试：Attrbute的初始化
    struct Attribute Attr1;
    Attr1.num=3;
    Attr1.name[0]="A1";
    Attr1.name[1]="A2";
    Attr1.name[2]="A3";
    Attr1.type[0]=-1;
    Attr1.type[1]=0;
    Attr1.type[2]=10;
    
    //测试：初始化table
    class Table table1("new_table",Attr1);
    
    //测试：连续插入table三个数据，其中第二个会由于类型不同而报错
    vector<Tuple>& tmp_tuples = table1.getTuple();
    tmp_tuples.push_back(T1);
    tmp_tuples.push_back(T2);
    tmp_tuples.push_back(T3);
    
    cout<<endl;
    
    //显示table里的重点信息
    table1.showTable();
    
    cout<<endl;
    
    //接下来进行index的操作，可以用 p table1.getIndex() 观察每一步的效果
    //插入index
    table1.setIndex(0, "ym");
    
    //插入index，由于该处已经有了index所以报错
    table1.setIndex(0, "mo");
    
    //插入index，由于该处已经有了同名index所以报错
    table1.setIndex(1, "ym");
    
    //插入另一个index
    table1.setIndex(1, "mo");
    
    //删除一个index，由于没有py的index，所以报错
    table1.dropIndex("py");
    
    //删除一个index，成功
    table1.dropIndex("ym");
    
    //再插入一个index
    table1.setIndex(0, "66");
    
    return 0;
}
