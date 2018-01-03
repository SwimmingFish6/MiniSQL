//
//  RecordManager.cpp
//  
//
//  Created by Β¬Πρ»Τ on 16/7/1.
//
//

#include "RecordManager.h"



RecordManager::~RecordManager()
{
    if (!curTableName.empty())
    {
        string filename;
        string num;
        stringstream ss;
        ss << curBlockNum;
        ss >> num;
        filename = curTableName + num;
        bm.PutBlock(temp, filename.c_str());
    }
}

/**
*
* create a table
* @param tableName: name of table
*/
void RecordManager::tableCreate(string tableName, int recordsize)
{
    if (!curTableName.empty())
    {
        string filename;
        string num;
        stringstream ss;
        ss << curBlockNum;
        ss >> num;
        filename = curTableName + num;
        bm.PutBlock(temp, filename.c_str());
    }
    curTableName = tableName;
    curBlockNum = 1;
    maxBlockNum = 1;
    recordSize = recordsize;
    string filename = curTableName + "1";
    temp = bm.GetBlock(filename.c_str());
    init_Block();
}

void RecordManager::tableLoad(string tableName, int recordsize, int maxblocknum)
{
    if (!curTableName.empty())
    {
        string filename;
        string num;
        stringstream ss;
        ss << curBlockNum;
        ss >> num;
        filename = curTableName + num;
        bm.PutBlock(temp, filename.c_str());
    }
    curTableName = tableName;
    curBlockNum = 1;
    maxBlockNum = maxblocknum;
    recordSize = recordsize;
    string filename = curTableName + "1";
    temp = bm.GetBlock(filename.c_str());
}

/**
*
* drop a table
* @param tableName: name of table
*/

void RecordManager::tableDrop(string tableName)
{
    for (int i = 1; i <= maxBlockNum; i++)
    {
        string filename;
        string num;
        stringstream ss;
        ss << i;
        ss >> num;
        filename = tableName + num;
        bm.Delete(filename.c_str());
        curTableName = "";
    }
}

/**
*
* insert a record to table
* @param tableName: name of table
* @param record: value of record
* @param recordSize: size of the record
* @return the position of block in the file(-1 represent error)
*/
Address RecordManager::recordInsert(string tableName, char* record)
{
    //if (curTableName == tableName)
    //{
    int freep;
    string filename;
    string num;
    stringstream ss;
    Address result;
    ss << curBlockNum;
    ss >> num;
    int isvalue = -1;
    filename = tableName + num;
    memcpy(&freep, temp.Data, 4);
    if (freep == 0)
    {
        int find = 0;
        bm.PutBlock(temp, filename.c_str());
        curBlockNum++;
        while (curBlockNum <= maxBlockNum)
        {
            stringstream ss;
            ss << curBlockNum;
            ss >> num;
            filename = tableName + num;
            temp = bm.GetBlock(filename.c_str());
            memcpy(&freep, temp.Data, 4);
            if (freep != 0)
            {
                find = 1;
                break;
            }
            bm.UnlockBlock(filename.c_str());
            curBlockNum++;
        }

        if (!find)
        {
            maxBlockNum = curBlockNum;
            init_Block();
            memcpy(&freep, temp.Data, 4);
        }
    }
    result.FileNumber = curBlockNum;
    result.BlockOffset = freep;
    memcpy(temp.Data + freep, record, recordSize);
    freep += recordSize;
    memcpy(temp.Data, temp.Data + freep, 4);
    memcpy(temp.Data + freep, &isvalue, 4);
    return result;
}

vector<record> RecordManager::recordAllDelete(string tableName, vector<Condition>* conditionVector, vector<Attribute>* attributeVector)
{
    vector<record> dellist;
    string filename;
    string num;
    stringstream ss;
    ss << curBlockNum;
    ss >> num;
    filename = tableName + num;
    bm.PutBlock(temp, filename.c_str());
    for (curBlockNum = 1; curBlockNum <=maxBlockNum; curBlockNum++)
    {
        stringstream ss;
        ss << curBlockNum;
        ss >> num;
        filename = tableName + num;
        temp = bm.GetBlock(filename.c_str());
        vector<record> del = recordBlockDelete(tableName, conditionVector, attributeVector);
        dellist.insert(dellist.end(), del.begin(), del.end());
        bm.PutBlock(temp, filename.c_str());
    }
    curBlockNum = 1;
    stringstream ssnew;
    ssnew << curBlockNum;
    ssnew >> num;
    filename = tableName + num;
    temp = bm.GetBlock(filename.c_str());
    return dellist;
}

vector<record> RecordManager::recordBlockDelete(string tableName, vector<Condition>* conditionVector, vector<Attribute>* attributeVector)
{
    vector<record> del;
    int recordOffset;
    int isvalue = -1;
    int maxoffset = BLOCKSIZE - recordSize;

    for (recordOffset = 4; recordOffset <= maxoffset; recordOffset += recordSize + 4)
    {
        int tempfreep;
        memcpy(&tempfreep, temp.Data + recordOffset + recordSize, 4);
        if (tempfreep != isvalue)
            continue;
        //if the recordBegin point to a record

        if (recordConditionFit(recordOffset, attributeVector, conditionVector))
        {
            record resultrecord;
            char* data = temp.Data + recordOffset;
            for (int i = 0; i < attributeVector->size(); i++)
            {
                int type = (*attributeVector)[i].getType();
                int typeSize = type <= 255 ? type : 4;
                if ((*attributeVector)[i].ifIndex())
                    memcpy(resultrecord.record[i], data, typeSize);
                data += typeSize;
            }
            del.push_back(resultrecord);
            memcpy(temp.Data + recordOffset + recordSize, temp.Data, 4);
            memcpy(temp.Data, &recordOffset, 4);
        }
    }

    return del;
}

vector<record> RecordManager::recordAllSelect(string tableName, vector<Condition>* conditionVector, vector<Attribute>* attributeVector)
{
    vector<record> result;

    string filename;
    string num;
    stringstream ss;
    ss << curBlockNum;
    ss >> num;
    filename = tableName + num;
    bm.PutBlock(temp, filename.c_str());
    for (curBlockNum = 1; curBlockNum <= maxBlockNum; curBlockNum++)
    {
        stringstream ss;
        ss << curBlockNum;
        ss >> num;
        filename = tableName + num;
        temp = bm.GetBlock(filename.c_str());
        vector<record> blockResult = recordBlockSelect(tableName, conditionVector, attributeVector);
        result.insert(result.end(), blockResult.begin(), blockResult.end());
        bm.UnlockBlock(filename.c_str());
    }
    curBlockNum = 1;
    stringstream ssnew;
    ssnew << curBlockNum;
    ssnew >> num;
    filename = tableName + num;
    temp = bm.GetBlock(filename.c_str());
    return result;
}

vector<record> RecordManager::recordIndexSelect(string tableName, Address addr, vector<Condition>* conditionVector, vector<Attribute>* attributeVector)
{
    vector<record> result;
    if (curBlockNum != addr.FileNumber)
    {
        string filename;
        string num;
        stringstream ss;
        ss << curBlockNum;
        ss >> num;
        filename = tableName + num;
        bm.PutBlock(temp, filename.c_str());
        curBlockNum = addr.FileNumber;
        stringstream ssnew;
        ssnew << curBlockNum;
        ssnew >> num;
        filename = tableName + num;
        temp = bm.GetBlock(filename.c_str());
    }
    if (recordConditionFit(addr.BlockOffset, attributeVector, conditionVector))
    {
        record resultrecord;
        char* data = temp.Data + addr.BlockOffset;
        for (int i = 0; i < attributeVector->size(); i++)
        {
            int type = (*attributeVector)[i].getType();
            int typeSize = type <= 255 ? type : 4;
            memcpy(resultrecord.record[i], data, typeSize);
            data += typeSize;
        }
        result.push_back(resultrecord);
    }
    return result;
}

vector<record> RecordManager::recordAllAddress(string tableName, vector<Attribute>* attributeVector, int attrIndex)
{
    vector<record> result;
    string filename;
    string num;
    stringstream ss;
    ss << curBlockNum;
    ss >> num;
    filename = tableName + num;
    bm.PutBlock(temp, filename.c_str());
    for (curBlockNum = 1; curBlockNum <= maxBlockNum; curBlockNum++)
    {
        stringstream ss;
        ss << curBlockNum;
        ss >> num;
        filename = tableName + num;
        temp = bm.GetBlock(filename.c_str());
        vector<record> blockResult = recordBlockAddress(curBlockNum, attributeVector, attrIndex);
        result.insert(result.end(), blockResult.begin(), blockResult.end());
        bm.UnlockBlock(filename.c_str());
    }
    curBlockNum = 1;
    stringstream ssnew;
    ssnew << curBlockNum;
    ssnew >> num;
    filename = tableName + num;
    temp = bm.GetBlock(filename.c_str());
    return result;
}

vector<record> RecordManager::recordBlockSelect(string tableName, vector<Condition>* conditionVector, vector<Attribute>* attributeVector)
{
    vector<record> result;

    int recordOffset;
    int isvalue = -1;
    int maxoffset = BLOCKSIZE - recordSize;
    int type;
    string attributeName;
    int typeSize;

    for (recordOffset = 4; recordOffset <= maxoffset; recordOffset += recordSize + 4)
    {
        int tempfreep;
        memcpy(&tempfreep, temp.Data + recordOffset + recordSize, 4);
        if (tempfreep != isvalue)
            continue;
        //if the recordBegin point to a record

        if (recordConditionFit(recordOffset, attributeVector, conditionVector))
        {
            record resultrecord;
            char* data = temp.Data + recordOffset;
            for (int i = 0; i < attributeVector->size(); i++)
            {
                type = (*attributeVector)[i].getType();
                typeSize = type <= 255 ? type : 4;
                memcpy(resultrecord.record[i], data, typeSize);
                data += typeSize;
            }
            result.push_back(resultrecord);
        }
    }

    return result;
}

vector<record> RecordManager::recordBlockAddress(int filenum, vector<Attribute>* attributeVector, int attrIndex)
{
    vector<record> result;

    int recordOffset;
    int isvalue = -1;
    int maxoffset = BLOCKSIZE - recordSize;
    int type;
    string attributeName;
    int typeSize;
    Address addr;
    addr.FileNumber = filenum;
    for (recordOffset = 4; recordOffset <= maxoffset; recordOffset += recordSize + 4)
    {
        int tempfreep;
        memcpy(&tempfreep, temp.Data + recordOffset + recordSize, 4);
        if (tempfreep != isvalue)
            continue;
        //if the recordBegin point to a record
        record resultrecord;
        int keyoffset = recordOffset;
        for (int i = 0; i < attrIndex; i++)
        {
            type = (*attributeVector)[i].getType();
            typeSize = type <= 255 ? type : 4;
            keyoffset += typeSize;
        }
        addr.BlockOffset = recordOffset;
        memcpy(resultrecord.record[0], &addr, sizeof(Address));
        type = (*attributeVector)[attrIndex].getType();
        typeSize = type <= 255 ? type : 4;
        memcpy(resultrecord.record[1], temp.Data + keyoffset, typeSize);
        result.push_back(resultrecord);
    }
    return result;
}

/**
*
* judge if the record meet the requirement
* @param recordBegin: point to a record
* @param recordSize: size of the record
* @param attributeVector: the attribute list of the record
* @param conditionVector: the conditions
* @return bool: if the record fit the condition
*/
bool RecordManager::recordConditionFit(int offset, vector<Attribute>* attributeVector, vector<Condition>* conditionVector)
{
    if (conditionVector == NULL || conditionVector->size() == 0)
    {
        return true;
    }
    int type;
    string attributeName;
    int typeSize;
    int typeOffset;
    char content[255];

    char *contentBegin = temp.Data + offset;
    for (int i = 0; i < conditionVector->size(); i++)
    {
        attributeName = (*conditionVector)[i].getAttributeName();
        typeOffset = 0;
        for (int j = 0; j < (*attributeVector).size(); j++)
        {
            type = (*attributeVector)[j].getType();
            typeSize = type <= 255 ? type : 4;
            if ((*attributeVector)[j].getName() == attributeName)
            {
                //if this attribute need to deal about the condition

                memcpy(content, contentBegin + typeOffset, typeSize);
                content[typeSize] = 0;
                if (!contentConditionFit(content, type, &(*conditionVector)[i]))
                {
                    //if this record is not fit the conditon
                    return false;
                }
            }
            typeOffset += typeSize;
        }
    }
    return true;
}

/**
*
* judge if the content meet the requirement
* @param content: point to content
* @param type: type of content
* @param condition: condition
* @return bool: the content if meet
*/
bool RecordManager::contentConditionFit(char* content, int type, Condition* condition)
{
    if (type == TYPE_INT)
    {
        //if the content is a int
        int tmp = *((int *)content);   //get content value by point
        return condition->ifRight(tmp);
    }
    else if (type == TYPE_FLOAT)
    {
        //if the content is a float
        float tmp = *((float *)content);   //get content value by point
        return condition->ifRight(tmp);
    }
    else
    {
        //if the content is a string
        return condition->ifRight(content);
    }
    return true;
}

void RecordManager::init_Block()
{
    int freep = 4, i;
    int maxoffset = BLOCKSIZE - recordSize - 4;
    memcpy(temp.Data, &freep, 4);
    for (i = 4; i <= maxoffset; i += recordSize + 4)
    {
        freep += recordSize + 4;
        memcpy(temp.Data + i + recordSize, &freep, 4);
    }
    memset(temp.Data + i - 4, 0, 4);
}
