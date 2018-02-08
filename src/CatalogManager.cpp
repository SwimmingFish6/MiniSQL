#include "CatalogManager.h"

int CatalogManager::getType(const char* tableName, const char *attributeName)
{
    //BufferManager bm;
    if (!findTable(tableName))
        return -1;
    Block block;
    char *address, *addressFind;
    int type;
    char fileName[100];
    strcpy(fileName, tableName);
    strcpy(fileName + strlen(tableName), ".db");
    block = bm.GetBlock(fileName);
    bm.UnlockBlock(fileName);
    
    address = block.Data;
    addressFind = findAttribute(address, attributeName);
    if (NULL == addressFind)
        return TYPE_ERROR;
    addressFind += 32;
    memcpy(&type, addressFind, sizeof(int));
    return type;
}

int CatalogManager::getIsUnique(const char* tableName, const char *attributeName)
{
    //BufferManager bm;
    if (!findTable(tableName))
        return -1;
    Block block;
    char *address, *addressFind;
    int isUnique;
    char fileName[100];
    strcpy(fileName, tableName);
    strcpy(fileName + strlen(tableName), ".db");
    block = bm.GetBlock(fileName);
    bm.UnlockBlock(fileName);

    address = block.Data;
    addressFind = findAttribute(address, attributeName);
    if (NULL == addressFind)
        return -1;
    addressFind += 32 + sizeof(int);
    memcpy(&isUnique, addressFind, sizeof(int));
    return isUnique;
}

int CatalogManager::getIndexNumber(const char* tableName, const char *attributeName)
{
    //BufferManager bm;
    if (!findTable(tableName))
        return -1;
    Block block;
    char *address, *addressFind;
    int indexNumber;
    char fileName[100];
    strcpy(fileName, tableName);
    strcpy(fileName + strlen(tableName), ".db");
    block = bm.GetBlock(fileName);
    bm.UnlockBlock(fileName);

    address = block.Data;
    addressFind = findAttribute(address, attributeName);
    if (NULL == addressFind)
        return -1;
    addressFind += 32 + sizeof(int) * 2;
    memcpy(&indexNumber, addressFind, sizeof(int));
    return indexNumber;
}

int CatalogManager::getNextIndexNumber(const char* tableName, const char *attributeName)
{
    //BufferManager bm;
    if (!findTable(tableName))
        return -1;
    Block block;
    char *address, *addressFind;
    int indexNumber;
    char fileName[100];
    strcpy(fileName, tableName);
    strcpy(fileName + strlen(tableName), ".db");
    block = bm.GetBlock(fileName);
    bm.UnlockBlock(fileName);

    address = block.Data;
    addressFind = findAttribute(address, attributeName);
    if (NULL == addressFind)
        return -1;
    addressFind += 32 + sizeof(int) * 3;
    memcpy(&indexNumber, addressFind, sizeof(int));
    return indexNumber;
}

bool CatalogManager::findTable(const char* tableName)
{
    char fileName[100];
    strcpy(fileName, tableName);
    strcpy(fileName + strlen(tableName), ".db");
    return bm.FindBlock(fileName);
}

int CatalogManager::addTable(const char* tableName, vector<Attribute> *attributeList, int primaryLocation)
{
    if (findTable(tableName))
        return -1;

    char fileName[100];
    Block block;
    strcpy(fileName, tableName);
    strcpy(fileName + strlen(tableName), ".db");
    block = bm.GetBlock(fileName);

    char *addressWrite = block.Data;
    int attributeSize = (int)attributeList->size();
    int recordNumber = 1;
    memcpy(addressWrite, &attributeSize, sizeof(int));
    addressWrite += sizeof(int);
    memcpy(addressWrite, &primaryLocation, sizeof(int));
    addressWrite += sizeof(int);
    memcpy(addressWrite, &recordNumber, sizeof(int));
    addressWrite += sizeof(int);

    for (int i = 0; i < attributeList->size(); i++) {
        memcpy(addressWrite, &((*attributeList)[i]), sizeof(Attribute));
        addressWrite += sizeof(Attribute);
    }

    bm.PutBlock(block, fileName);
    return 0;
}

int CatalogManager::dropTable(const char* tableName)
{
    char fileName[100];
    strcpy(fileName, tableName);
    strcpy(fileName + strlen(tableName), ".db");
    //printf("%s\n", fileName);
    bm.Delete(fileName);
    return 1;
}

char *CatalogManager::findAttribute(const char *address, const char *attributeName)
{
    char name[32];
    int size;
    memcpy(&size, address, sizeof(int));
    address += sizeof(int) * 3; //size, primaryLocation, recordNumber
    for (int i = 0; i < size; i++) {
        memcpy(name, address, 32);
        if (!strcmp(name, attributeName))
            return (char*)address;
        address += sizeof(Attribute);
    }
    return NULL;
}

vector<IndexInfo> *CatalogManager::getAllIndex(const char* tableName, vector<IndexInfo> *indexList)
{
    //BufferManager bm;
    Block block;
    char *address, name[32];
    int size, type, indexNumber, nextIndexNumber;

    char fileName[100], indexName[32];
    strcpy(fileName, tableName);
    strcpy(fileName + strlen(tableName), ".db");
    block = bm.GetBlock(fileName);
    bm.UnlockBlock(fileName);

    address = block.Data;
    memcpy(&size, address, sizeof(int));
    address += sizeof(int) * 3; //size, primaryLocation, recordNumber
    indexList->clear();
    for (int i = 0; i < size; i++) {
        memcpy(name, address, 32);
        address += 32;
        memcpy(&type, address, sizeof(int));
        address += sizeof(int) * 2;
        memcpy(&indexNumber, address, sizeof(int));
        address += sizeof(int);
        memcpy(&nextIndexNumber, address, sizeof(int));
        address += sizeof(int);
        memcpy(indexName, address, 32);
        address += 32;
        IndexInfo indexNode(tableName, indexNumber, nextIndexNumber, name, type, indexName);
        indexList->push_back(indexNode);
    }
    return indexList;
}

int CatalogManager::alterIndex(const char* tableName, const char *attributeName, int indexNumber, int nextIndexNumber)
{
    //BufferManager bm;
    if (!findTable(tableName))
        return -1;
    Block block;
    char *address, *addressWrite;
    char fileName[100];
    strcpy(fileName, tableName);
    strcpy(fileName + strlen(tableName), ".db");
    block = bm.GetBlock(fileName);

    address = block.Data;
    addressWrite = findAttribute(address, attributeName);
    if (NULL == addressWrite)
        return -1;
    addressWrite += 32 + sizeof(int) * 2;
    memcpy(addressWrite, &indexNumber, sizeof(int));
    addressWrite += sizeof(int);
    memcpy(addressWrite, &nextIndexNumber, sizeof(int));

    bm.PutBlock(block, fileName);
    return 0;
}

int CatalogManager::revokeIndex(const char* tableName, const char *attributeName)
{
    return alterIndex(tableName, attributeName, -1, -1);
}

int CatalogManager::calculateSize(const char* tableName)
{
    //BufferManager bm;
    Block block;
    char *address;
    int size, type, totalSize;
    char fileName[100];
    strcpy(fileName, tableName);
    strcpy(fileName + strlen(tableName), ".db");
    block = bm.GetBlock(fileName);
    bm.UnlockBlock(fileName);

    address = block.Data;
    memcpy(&size, address, sizeof(int));
    totalSize = 0;
    address += sizeof(int) * 3; //size, primaryLocation, recordNumber
    for (int i = 0; i < size; i++) {
        address += 32;
        memcpy(&type, address, sizeof(int));
        address += sizeof(int) * 4 + 32;
        totalSize += typeSize(type);
    }
    return totalSize;
}

int CatalogManager::typeSize(int type)
{
    if (TYPE_ERROR == type)
        return 0;
    else if (TYPE_INT == type)
        return sizeof(int);
    else if (TYPE_FLOAT == type)
        return sizeof(float);
    else if (type > TYPE_FLOAT)
        return 0;
    else return type;
}

int CatalogManager::getAttributeSize(const char *tableName)
{
    if (!findTable(tableName))
        return -1;

    Block block;
    int size;
    char *address;
    char fileName[100];
    strcpy(fileName, tableName);
    strcpy(fileName + strlen(tableName), ".db");
    block = bm.GetBlock(fileName);
    bm.UnlockBlock(fileName);

    address = block.Data;
    memcpy(&size, address, sizeof(int));
    return size;
}

int CatalogManager::getAttribute(char *attribute, const char *tableName, int attributeLocation)
{
    if (!findTable(tableName))
        return -1;

    Block block;
    char *address;
    char fileName[100];
    strcpy(fileName, tableName);
    strcpy(fileName + strlen(tableName), ".db");
    block = bm.GetBlock(fileName);
    bm.UnlockBlock(fileName);

    address = block.Data;
    int size;
    memcpy(&size, address, sizeof(int));
    address += sizeof(int) * 3;
    if (attributeLocation >= size) {
        attribute = NULL;
        return -1;
    }
    address += attributeLocation * sizeof(Attribute);
    memcpy(attribute, address, 32);
    return 0;
}

int CatalogManager::getPrimaryKey(char *primary, const char *tableName)
{
    if (!findTable(tableName))
        return -1;

    Block block;
    char *address;
    char fileName[100];
    strcpy(fileName, tableName);
    strcpy(fileName + strlen(tableName), ".db");
    block = bm.GetBlock(fileName);
    bm.UnlockBlock(fileName);

    address = block.Data;
    int size, primaryLocation;
    memcpy(&size, address, sizeof(int));
    address += sizeof(int);
    memcpy(&primaryLocation, address, sizeof(int));
    address += sizeof(int) * 2;
    if (primaryLocation >= size) {
        primary = NULL;
        return -1;
    }
    address += primaryLocation * sizeof(Attribute);
    memcpy(primary, address, 32);
    return 0;
}

int CatalogManager::getRecordNumber(const char *tableName)
{
    if (!findTable(tableName))
        return -1;

    Block block;
    int recordNumber;
    char *address;
    char fileName[100];
    strcpy(fileName, tableName);
    strcpy(fileName + strlen(tableName), ".db");
    block = bm.GetBlock(fileName);
    bm.UnlockBlock(fileName);

    address = block.Data;
    address += sizeof(int) * 2;
    memcpy(&recordNumber, address, sizeof(int));
    return recordNumber;
}

int CatalogManager::setRecordNumber(const char *tableName, int recordNumber)
{
    if (recordNumber < 0)
        return -1;
    if (!findTable(tableName))
        return -1;

    Block block;
    char *address;
    char fileName[100];
    strcpy(fileName, tableName);
    strcpy(fileName + strlen(tableName), ".db");
    block = bm.GetBlock(fileName);

    address = block.Data;
    address += sizeof(int) * 2;
    memcpy(address, &recordNumber, sizeof(int));
    bm.PutBlock(block, fileName);
    return 0;
}

int CatalogManager::addIndex(const char* tableName, const char *attributeName, const char *indexName)
{
    int indexNumber = 1, nextIndexNumber = 2;
    if (!findTable(tableName))
        return -1;
    Block block;
    char *address, *addressWrite;
    char fileName[100];
    strcpy(fileName, tableName);
    strcpy(fileName + strlen(tableName), ".db");
    block = bm.GetBlock(fileName);

    address = block.Data;
    addressWrite = findAttribute(address, attributeName);
    if (NULL == addressWrite)
        return -1;
    addressWrite += 32 + sizeof(int) * 2;
    memcpy(addressWrite, &indexNumber, sizeof(int));
    addressWrite += sizeof(int);
    memcpy(addressWrite, &nextIndexNumber, sizeof(int));
    addressWrite += sizeof(int);
    memcpy(addressWrite, indexName, 32);

    bm.PutBlock(block, fileName);
    return 0;
}

vector<Attribute> *CatalogManager::getAllAttritube(const char* tableName, vector<Attribute> *attributeList)
{
    //BufferManager bm;
    Block block;
    char name[32];
    int type;
    int isUnique;
    int indexNumber;
    int nextIndexNumber;
    char indexName[32];
    int size;
    char *address;
    char fileName[100];
    strcpy(fileName, tableName);
    strcpy(fileName + strlen(tableName), ".db");
    block = bm.GetBlock(fileName);
    bm.UnlockBlock(fileName);

    address = block.Data;
    memcpy(&size, address, sizeof(int));
    address += sizeof(int) * 3; //size, primaryLocation, recordNumber
    attributeList->clear();
    for (int i = 0; i < size; i++) {
        memcpy(name, address, 32);
        address += 32;
        memcpy(&type, address, sizeof(int));
        address += sizeof(int);
        memcpy(&isUnique, address, sizeof(int));
        address += sizeof(int);
        memcpy(&indexNumber, address, sizeof(int));
        address += sizeof(int);
        memcpy(&nextIndexNumber, address, sizeof(int));
        address += sizeof(int);
        memcpy(indexName, address, 32);
        address += 32;
        Attribute attribute(name, type, isUnique, indexNumber, nextIndexNumber, indexName);
        attributeList->push_back(attribute);
    }
    return attributeList;
}
