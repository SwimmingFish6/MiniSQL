//
//  RecordManager.h
//  
//
//  Created by Β¬Πρ»Τ on 16/7/1.
//
//

#ifndef ____RecordManager__
#define ____RecordManager__


#include <string>
#include <vector>
#include <cstring>
#include "condition.h"
#include <cstdbool>
#include "BufferManager.h"
#include "Attribute.h"
using namespace std;

typedef struct{
    char record[32][256];
} record;


class RecordManager{
private:
    Block temp;
    BufferManager& bm;
    string curTableName;
    int curBlockNum;
    int maxBlockNum;
    int recordSize;
    void init_Block();
    vector<record> recordBlockDelete(string tableName, vector<Condition>* conditionVector, vector<Attribute>* attributeVector);
    vector<record> recordBlockSelect(string tableName, vector<Condition>* conditionVector, vector<Attribute>* attributeVector);
    vector<record> recordBlockAddress(int filenum, vector<Attribute>* attributeVector, int attrIndex);
    bool recordConditionFit(int offset, vector<Attribute>* attributeVector, vector<Condition>* conditionVector);
    bool contentConditionFit(char* content, int type, Condition* condition);
    
public:
    RecordManager(BufferManager& buffer) : bm(buffer){ }
    ~RecordManager();
    void tableCreate(string tableName, int recordsize);
    void tableLoad(string tableName, int recordsize, int maxblocknum);
    void tableDrop(string tableName);
    Address recordInsert(string tableName,char* record);
    vector<record> recordAllDelete(string tableName, vector<Condition>* conditionVector, vector<Attribute>* attributeVector);
    vector<record> recordAllSelect(string tableName, vector<Condition>* conditionVector, vector<Attribute>* attributeVector);
    vector<record> recordIndexSelect(string tableName, Address addr, vector<Condition>* conditionVector, vector<Attribute>* attributeVector);
    vector<record> recordAllAddress(string tableName, vector<Attribute>* attributeVector, int attrIndex);
    string getCurTabelName() { return curTableName; }
    int getMaxBlockNum() { return maxBlockNum; }
};


#endif /* defined(____RecordManager__) */
