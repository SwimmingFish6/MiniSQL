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
���� CREATE_TABLE    value1 = 0--���� 1--������ 2--primary key
value2 Ϊ type
value3 Ϊ�Ƿ�unique
���� CREATE_INDEX    value1 = 0--������ 1--���� 2--������
���� SELECT          value1 = 0--������ 1--����
���� INSERT          value1 = 0--���� 1--���� value2 = 0--int or float 1--string
���� DELETE          value1 = 0--����
���� DROP_TABLE      value1 = 0--����
���� DROP_INDEX      value1 = 0--������
���� EXECFILE        value1 = 0--�ļ���
���� QUIT
���� ILLEGAL
*/
struct name
{
    string word;
    int value1;
    int value2;
    int value3;
};