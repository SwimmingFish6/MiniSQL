#pragma once

#include <string>
using namespace std;

#define BLOCKSIZE 4096
#define NUMOFBLOCK 100

typedef struct
{
    int FileNumber;
    int BlockOffset;
} Address;

typedef struct
{
    char Data[BLOCKSIZE];
} Block;

/*
对于 CREATE_TABLE    value1 = 0--表名 1--属性名 2--primary key
value2 为 type
value3 为是否unique
对于 CREATE_INDEX    value1 = 0--索引名 1--表名 2--属性名
对于 SELECT          value1 = 0--属性名 1--表名
对于 INSERT          value1 = 0--表名 1--数据 value2 = 0--int or float 1--string
对于 DELETE          value1 = 0--表名
对于 DROP_TABLE      value1 = 0--表名
对于 DROP_INDEX      value1 = 0--索引名
对于 EXECFILE        value1 = 0--文件名
对于 QUIT
对于 ILLEGAL
*/
struct name
{
    string word;
    int value1;
    int value2;
    int value3;
};