#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <io.h>
#include "Attribute.h"
#include "BufferManager.h"
#include "IndexInfo.h"
using namespace std;

#define UNIQUE_ERROR (-1);

class CatalogManager {
private:
    BufferManager& bm;
public:
    CatalogManager(BufferManager& buffer) : bm(buffer) {}
    virtual ~CatalogManager() {}
    int getType(const char* tableName, const char *attributeName);
    int getIsUnique(const char* tableName, const char *attributeName);
    int getIndexNumber(const char* tableName, const char *attributeName);
    int getNextIndexNumber(const char* tableName, const char *attributeName);
    int getAttributeSize(const char *tableName);
    int getAttribute(char *attribute, const char *tableName, int attributeLocation);
    int getPrimaryKey(char *primary, const char *tableName);
    int getRecordNumber(const char *tableName);
    int setRecordNumber(const char *tableName, int recordNumber);
    bool findTable(const char* tableName);
    int addTable(const char* tableName, vector<Attribute> *attributeList, int primaryLocation);
    int dropTable(const char* tableName);
    int addIndex(const char* tableName, const char *attributeName, const char *indexName);
    int alterIndex(const char* tableName, const char *attributeName, int indexNumber, int nextIndexNumber);
    int revokeIndex(const char* tableName, const char *attributeName);
    vector<IndexInfo> *getAllIndex(const char* tableName, vector<IndexInfo> *indexList);
    int calculateSize(const char* tableName);
    vector<Attribute> *getAllAttritube(const char* tableName, vector<Attribute> *attributeList);
private:
    int typeSize(int type);
    char *findAttribute(const char *address, const char *attributeName);
};
