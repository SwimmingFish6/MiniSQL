#pragma once

#include <string>
using namespace std;

class IndexInfo {
private:
    int indexNumber;
    int nextAvailableNumber;
    char tableName[100];
    char attribute[32];
    int type;
    char indexName[32];
public:
    IndexInfo(const char *tableName, int indexNumber, int nextIndexNumber, const char *attribute, int type, const char *indexName)
        :indexNumber(indexNumber), nextAvailableNumber(nextIndexNumber), type(type)
    {
        memcpy(this->tableName, tableName, 100);
        memcpy(this->attribute, attribute, 32);
        memcpy(this->indexName, indexName, 32);
    }
    int getIndexNumber() { return indexNumber; }
    int getNextIndexNumber() { return nextAvailableNumber; }
    char *getTableName() { return tableName; }
    char *getAttritube() { return attribute; }
    int getType() { return type; }
    char *getIndexName() { return indexName; }
    int setIndexNumber(int indexNumber) { this->indexNumber = indexNumber; }
    int setNextIndexNumber(int nextIndexNumber) { nextAvailableNumber = nextIndexNumber; }
};