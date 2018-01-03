#include "API.h"

int API::CreateTable(vector<name>& nameList)
{
    if (Catalog->findTable(nameList[0].word.c_str()))
        return TABLE_NAME_EXIST;
    curTableName = nameList[0].word;
    vector<Attribute> attrlist;
    char str[32];
    int pkindex;
    int recordsize = 0;
    for (int i = 1; i < nameList.size(); i++)
    {
        if (nameList[i].value1 == 4)
            continue;
        if (nameList[i].word.length() >= 32)
            return FIELD_NAME_TOO_LONG;
        if (nameList[i].value1 == 2)
            pkindex = i;
        memcpy(str, nameList[i].word.c_str(), 32);
        attrlist.push_back(Attribute(str, nameList[i].value2, nameList[i].value3));
        recordsize += nameList[i].value2 <= 255 ? nameList[i].value2 : 4;
    }
    Catalog->addTable(nameList[0].word.c_str(), &attrlist, pkindex - 1);
    Record->tableCreate(nameList[0].word, recordsize);
    TableSet.push_back(nameList[0].word);
    string pkindexname = nameList[0].word + nameList[pkindex].word;
    Catalog->addIndex(nameList[0].word.c_str(), nameList[pkindex].word.c_str(), pkindexname.c_str());
    Index->CreateIndex(nameList[pkindex].value2, pkindexname.c_str());
    return SUCCESS;
}

int API::DropTable(vector<name>& nameList)
{
    if (!Catalog->findTable(nameList[0].word.c_str()))
        return TABLE_NAME_NOT_EXIST;
    vector<IndexInfo> indexinfo;
    Catalog->getAllIndex(nameList[0].word.c_str(), &indexinfo);
    for (int i = 0; i < indexinfo.size(); i++)
        if (indexinfo[i].getIndexNumber() > 0)
            Index->DropIndex(indexinfo[i].getType(), indexinfo[i].getIndexNumber(), indexinfo[i].getNextIndexNumber(), indexinfo[i].getIndexName());
    if (Record->getCurTabelName() != nameList[0].word)
    {
        Record->tableLoad(nameList[0].word, Catalog->calculateSize(nameList[0].word.c_str()), Catalog->getRecordNumber(nameList[0].word.c_str()));
        TableSet.push_back(nameList[0].word);
    }
    Record->tableDrop(nameList[0].word);
    TableSet.remove(nameList[0].word);
    Catalog->dropTable(nameList[0].word.c_str());
    return SUCCESS;
}

int API::CreateIndex(vector<name>& nameList)
{
    if (!Catalog->findTable(nameList[1].word.c_str()))
        return TABLE_NAME_NOT_EXIST;
    curTableName = nameList[1].word;
    vector<IndexInfo> indexinfo;
    vector<Attribute> attr;
    Catalog->getAllIndex(nameList[1].word.c_str(), &indexinfo);
    Catalog->getAllAttritube(nameList[1].word.c_str(), &attr);
    for (int i = 0; i < indexinfo.size(); i++)
        if (nameList[0].word == indexinfo[i].getIndexName() && indexinfo[i].getIndexNumber() > 0)
            return INDEX_NAME_EXIST;
        else if (nameList[2].word == indexinfo[i].getAttritube() && indexinfo[i].getIndexNumber() > 0)
            return FIELD_INEDX_EXIST;
    Catalog->addIndex(nameList[1].word.c_str(), nameList[2].word.c_str(), nameList[0].word.c_str());
    int typesize = nameList[2].value2 <= 255 ? nameList[2].value2 : 4;
    Index->CreateIndex(nameList[2].value2, nameList[0].word.c_str());
    int indexoffset;
    for (int i = 0; i < attr.size(); i++)
        if (nameList[2].word == attr[i].getName())
        {
            indexoffset = i;
            break;
        }
    vector<record> key;
    key = Record->recordAllAddress(nameList[1].word.c_str(), &attr, indexoffset);
    for (int i = 0; i < key.size(); i++)
    {
        Address addr;
        memcpy(&addr, key[i].record[0], sizeof(Address));
        Index->Insert(nameList[0].word.c_str(), addr, key[i].record[1]);
    }
    return SUCCESS;
}

int API::DropIndex(vector<name>& nameList)
{
    vector<IndexInfo> indexinfo;
    if (curTableName.empty())
        return INDEX_NAME_NOT_EXIST;
    Catalog->getAllIndex(curTableName.c_str(), &indexinfo);
    int i;
    for (i = 0; i < indexinfo.size(); i++)
        if (nameList[0].word == indexinfo[i].getIndexName())
            break;
    if (i == indexinfo.size())
        return INDEX_NAME_NOT_EXIST;
    Index->DropIndex(indexinfo[i].getType(), indexinfo[i].getIndexNumber(), indexinfo[i].getNextIndexNumber(), indexinfo[i].getIndexName());
    Catalog->revokeIndex(indexinfo[i].getTableName(), indexinfo[i].getAttritube());
    return SUCCESS;
}

int API::Select(vector<name>& nameList, vector<Condition>& conditionList, vector<record>& selectList)
{
    bool allselectdone;
    selectList.clear();
    string tablename = nameList[nameList.size() - 1].word;
    if (!Catalog->findTable(tablename.c_str()))
        return TABLE_NAME_NOT_EXIST;
    curTableName = tablename;
    vector<IndexInfo> indexinfo;
    vector<Attribute> attr;
    vector<record> temp;
    Catalog->getAllIndex(tablename.c_str(), &indexinfo);
    Catalog->getAllAttritube(tablename.c_str(), &attr);
    if (nameList[0].word != "*")
    {
        for (int i = 0; i < nameList.size() - 1; i++)
        {
            int j;
            for (j = 0; j < indexinfo.size(); j++)
                if (nameList[i].word == indexinfo[j].getAttritube())
                    break;
            if (j == indexinfo.size())
                return FIELD_NAME_NOT_EXIST;
        }
    }
    for (int i = 0; i < (int)conditionList.size() - 1; i++)
    {
        int j;
        for (j = 0; j < indexinfo.size(); j++)
            if (conditionList[i].getAttributeName() == indexinfo[j].getAttritube())
            {
                if (checkValues(conditionList[i].getValue(), indexinfo[j].getType()) == TYPE_ERROR)
                    return VALUE_TYPE_ERROR;
                break;
            }
        if (j == indexinfo.size())
            return FIELD_NAME_NOT_EXIST;
    }
    if (Record->getCurTabelName() != tablename)
    {
        Record->tableLoad(tablename, Catalog->calculateSize(tablename.c_str()), Catalog->getRecordNumber(tablename.c_str()));
        TableSet.push_back(tablename);
    }
    allselectdone = false;
    for (int i = 0; i < conditionList.size(); i++)
    {
        for (int j = 0; j < indexinfo.size(); j++)
        {
            if (conditionList[i].getAttributeName() == indexinfo[j].getAttritube())
            {
                if (indexinfo[j].getType() <= 255)
                {
                    string str = conditionList[i].getValue();
                    conditionList[i] = Condition(conditionList[i].getAttributeName(), str.substr(1, str.length() - 2), conditionList[i].getOp());
                }
                break;
            }
        }
    }
    for (int i = 0; i < conditionList.size(); i++)
    {
        if (conditionList[i].getOp() != CONDITION_EQUAL)
            continue;
        for (int j = 0; j < indexinfo.size(); j++)
        {
            if (conditionList[i].getAttributeName() == indexinfo[j].getAttritube() && indexinfo[j].getIndexNumber() > 0)
            {
                if (Index->isLoadedIndex(indexinfo[j].getIndexName()) == false)
                    Index->LoadIndex(indexinfo[j].getType(), indexinfo[j].getIndexNumber(), indexinfo[j].getNextIndexNumber(), indexinfo[j].getIndexName());
                int data_int;
                float data_float;
                char data_char[255];
                const char* datap;
                if (indexinfo[j].getType() <= 255)
                {
                    memcpy(data_char, conditionList[i].getValue().c_str(), 255);
                    datap = data_char;
                }
                else if (indexinfo[j].getType() == TYPE_INT)
                {
                    stringstream ss;
                    ss << conditionList[i].getValue();
                    ss >> data_int;
                    datap = (const char*)&data_int;
                }
                else if (indexinfo[j].getType() == TYPE_FLOAT)
                {
                    stringstream ss;
                    ss << conditionList[i].getValue();
                    ss >> data_float;
                    datap = (const char*)&data_float;
                }
                Address addr = Index->Select(indexinfo[j].getIndexName(), datap);
                if (addr.FileNumber == 0)
                    return SELECT_NOT_FIND;
                temp = Record->recordIndexSelect(tablename, addr, &conditionList, &attr);
                allselectdone = true;
                break;
            }
        }
        if (allselectdone)
            break;
    }
    if (!allselectdone)
        temp = Record->recordAllSelect(tablename.c_str(), &conditionList, &attr);
    if (temp.size() == 0)
        return SELECT_NOT_FIND;
    if (nameList.size() == 2 && nameList[0].word == "*")
    {
        selectList = temp;
        return SUCCESS;
    }
    for (int i = 0; i < temp.size(); i++)
        selectList.push_back(temp[i]);
    for (int i = 0; i < nameList.size() - 1; i++)
    {
        for (int j = 0; j < attr.size(); j++)
        {
            if (attr[j].getName() == nameList[i].word)
            {
                int typesize = nameList[i].value2 <= 255 ? nameList[i].value2 : 4;
                for (int k = 0; k < temp.size(); k++)
                {
                    memcpy(selectList[k].record[i], temp[k].record[j], typesize);
                    selectList[k].record[i][typesize] = 0;
                }
                break;
            }
        }
    }
    return SUCCESS;
}

int API::Insert(vector<name>& nameList)
{
    if (!Catalog->findTable(nameList[0].word.c_str()))
        return TABLE_NAME_NOT_EXIST;
    curTableName = nameList[0].word;
    vector<IndexInfo> indexinfo;
    vector<Attribute> attr;
    Catalog->getAllIndex(nameList[0].word.c_str(), &indexinfo);
    Catalog->getAllAttritube(nameList[0].word.c_str(), &attr);
    if (nameList.size() - 1 != attr.size())
        return ATTRIBUTE_COUNT_ERROR;
    int insertsize = 0;
    for (int i = 0; i < attr.size(); i++)
    {
        if (attr[i].getType() != nameList[i + 1].value2)
            return ATTRIBUTE_TYPE_ERROR;
        insertsize += attr[i].getType() <= 255 ? attr[i].getType() : 4;
        if (attr[i].ifUnique())
        {
            vector<Condition> cond;
            cond.push_back(Condition(attr[i].getName(), nameList[i + 1].word, CONDITION_EQUAL));
            if (Record->getCurTabelName() != nameList[0].word)
            {
                Record->tableLoad(nameList[0].word, Catalog->calculateSize(nameList[0].word.c_str()), Catalog->getRecordNumber(nameList[0].word.c_str()));
                TableSet.push_back(nameList[0].word);
            }
            if (Record->recordAllSelect(nameList[0].word, &cond, &attr).size() != 0)
                return UNIQUE_ERROR;
        }
    }
    char* insertdata = new char[insertsize];
    int recordoffset = 0;
    for (int i = 1; i < nameList.size(); i++)
    {
        if (nameList[i].value2 <= 255)
        {
            memcpy(insertdata + recordoffset, nameList[i].word.c_str(), nameList[i].value2);
        }
        else if (nameList[i].value2 == TYPE_INT)
        {
            int num;
            stringstream ss;
            ss << nameList[i].word;
            ss >> num;
            memcpy(insertdata + recordoffset, &num, 4);
        }
        else if (nameList[i].value2 == TYPE_FLOAT)
        {
            float num;
            stringstream ss;
            ss << nameList[i].word;
            ss >> num;
            memcpy(insertdata + recordoffset, &num, 4);
        }
        recordoffset += nameList[i].value2 <= 255 ? nameList[i].value2 : 4;
    }
    if (Record->getCurTabelName() != nameList[0].word)
    {
        Record->tableLoad(nameList[0].word, Catalog->calculateSize(nameList[0].word.c_str()), Catalog->getRecordNumber(nameList[0].word.c_str()));
        TableSet.push_back(nameList[0].word);
    }
    Address addr = Record->recordInsert(nameList[0].word, insertdata);
    delete insertdata;
    for (int i = 0; i < indexinfo.size(); i++)
        if (indexinfo[i].getIndexNumber() > 0)
        {
            if (Index->isLoadedIndex(indexinfo[i].getIndexName()) == false)
                Index->LoadIndex(indexinfo[i].getType(), indexinfo[i].getIndexNumber(), indexinfo[i].getNextIndexNumber(), indexinfo[i].getIndexName());
            int data_int;
            float data_float;
            const char* datap;
            if (indexinfo[i].getType() <= 255)
                datap = nameList[i + 1].word.c_str();
            else if (indexinfo[i].getType() == TYPE_INT)
            {
                stringstream ss;
                ss << nameList[i + 1].word.c_str();
                ss >> data_int;
                datap = (const char*)&data_int;
            }
            else if (indexinfo[i].getType() == TYPE_FLOAT)
            {
                stringstream ss;
                ss << nameList[i + 1].word.c_str();
                ss >> data_float;
                datap = (const char*)&data_float;
            }
            Index->Insert(indexinfo[i].getIndexName(), addr, datap);
        }
    return SUCCESS;
}

int API::Delete(vector<name>& nameList, vector<Condition>& conditionList)
{
    if (!Catalog->findTable(nameList[0].word.c_str()))
        return TABLE_NAME_NOT_EXIST;
    curTableName = nameList[0].word;
    vector<IndexInfo> indexinfo;
    vector<Attribute> attr;
    Catalog->getAllIndex(nameList[0].word.c_str(), &indexinfo);
    Catalog->getAllAttritube(nameList[0].word.c_str(), &attr);
    for (int i = 0; i < (int)conditionList.size() - 1; i++)
    {
        int j;
        for (j = 0; j < indexinfo.size(); j++)
            if (conditionList[i].getAttributeName() == indexinfo[j].getAttritube())
                break;
        if (j == indexinfo.size())
            return FIELD_NAME_NOT_EXIST;
    }
    if (Record->getCurTabelName() != nameList[0].word)
    {
        Record->tableLoad(nameList[0].word, Catalog->calculateSize(nameList[0].word.c_str()), Catalog->getRecordNumber(nameList[0].word.c_str()));
        TableSet.push_back(nameList[0].word);
    }
    for (int i = 0; i < conditionList.size(); i++) {
        for (int j = 0; j < indexinfo.size(); j++) {
            if (conditionList[i].getAttributeName() == indexinfo[j].getAttritube()) {
                if (indexinfo[j].getType() <= 255) {
                    string str = conditionList[i].getValue();
                    conditionList[i] = Condition(conditionList[i].getAttributeName(), str.substr(1, str.length() - 2), conditionList[i].getOp());
                }
                break;
            }
        }
    }
    vector<record> deletelist = Record->recordAllDelete(nameList[0].word, &conditionList, &attr);
    if (deletelist.size() == 0)
        return DELETE_NOT_EXIST;
    for (int i = 0; i < indexinfo.size(); i++)
    {
        if (indexinfo[i].getIndexName() > 0)
        {
            if (Index->isLoadedIndex(indexinfo[i].getIndexName()) == false)
                Index->LoadIndex(indexinfo[i].getType(), indexinfo[i].getIndexNumber(), indexinfo[i].getNextIndexNumber(), indexinfo[i].getIndexName());
            for (int j = 0; j < deletelist.size(); j++)
            {
                Index->Delete(indexinfo[i].getIndexName(), deletelist[j].record[i]);
            }
        }
    }
    return SUCCESS;
}

void API::CatalogDone()
{
    Catalog->setRecordNumber(Record->getCurTabelName().c_str(), Record->getMaxBlockNum());
    vector<IndexSet> set = Index->GetAllIndexSet();
    list<string>::iterator it;
    vector<IndexInfo> indexinfo;
    bool onesetdone;
    for (int i = 0; i < set.size(); i++)
    {
        onesetdone = false;
        for (it = TableSet.begin(); it != TableSet.end(); it++)
        {
            Catalog->getAllIndex(it->c_str(), &indexinfo);
            for (int j = 0; j < indexinfo.size(); j++)
            {
                if (indexinfo[j].getIndexNumber() > 0 && set[i].name == indexinfo[j].getIndexName())
                {
                    Catalog->alterIndex(indexinfo[j].getTableName(), indexinfo[j].getAttritube(), set[i].root, set[i].next);
                    onesetdone = true;
                    break;
                }
            }
            if (onesetdone)
                break;
        }
    }
}

int API::checkValues(string s, int type)
{
    int i = 0, flag = 0;
    if (256 == type)
    {
        if ('-' == s[0])
        {
            i++;
            if (1 == s.length())
                return 0;
        }
        while (i < s.length())
        {
            if ('0' > s[i] || '9' < s[i])
                return 0;
            i++;
        }
    }
    else if (257 == type)
    {
        if ('.' == s[0])
            return 0;
        if ('-' == s[0])
        {
            i++;
            if ('.' == s[i])
                return 0;
            if (1 == s.length())
                return 0;
        }
        while (i < s.length())
        {
            if ('.' == s[i])
            {
                if (flag || s.length() - 1 == i)
                    return 0;
                flag = 1;
            }
            else if ('0' > s[i] || '9' < s[i])
                return 0;
            i++;
        }
    }
    else
    {
        if ('\'' == s[0] && '\'' == s[0] && type == s.length() - 2)
            return type;
        else return 0;
    }
    return type;
}

