### MiniSQL

> author: 黄雨生 宋融 薛伟
>
> date: 2017.6

#### 1、编译

- Linux、Mac以及unix平台下，直接使用提供的makefile编译即可
- windows平台请通过VS自行搭建工程并编译运行，各文件之间的依赖关系请参照makefile

#### 2、使用

成功编译后在终端中输入：

```
./main
```

即可运行程序。(需在工程目录下，如果不在需要更改路径)

本MiniSQL支持标准的SQL语法，基本操作如下：

**创建表**

```mysql
create table 表名 (
	列名 类型 ,
	列名 类型 ,
	列名 类型 ,
	primary key (列名)
);
```

**删除表**

```mssql
drop table 表名;
```

**创建索引**

```mysql
create index 索引名 on 表名 (列名);
```

**删除索引**

```mysq
drop index 索引名;
```

**选择语句**

```mysq
drop table 表名; 或者 select * from 表名 where 条件 ;
```

**插入语句**

```my
insert into 表名 values ( 值1 , 值2 , … , 值n );
```

**删除记录**

```mysq
delete from 表名; 或者 delete from 表名 where 条件;
```

**执行文件**

```mysq
execfile 文件名;
```

**退出**

```mysq
exit;
```

#### 3、测试文件

我们也提供了一些测试文件供用户参考。工程中共包含两个sql文件：

- test_file文件包含1万条记录，插入时间较长
- test_file1文件包含1千条记录，插入速度较快

两个测试文件均会建立一个test表，并向其中插入数据(无主键冲突)。用户可根据自己的需求自由选择。