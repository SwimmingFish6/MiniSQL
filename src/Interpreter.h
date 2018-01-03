#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "condition.h"
#include "API.h"
#include "DataStructure.h"
#include "CatalogManager.h"
using namespace std;

#define NOTDIGIT (0)

#define CREATE_TABLE     (1)
#define CREATE_INDEX     (2)
#define SELECT           (3)
#define INSERT           (4)
#define DELETE           (5)
#define DROP_TABLE       (6)
#define DROP_INDEX       (7)
#define EXECFILE         (-3)  //与API中操作结果值相分离
#define QUIT             (-4)  //与API中操作结果值相分离
#define ILLEGAL          (-1)
#define ATTRIBUTEERROR   (-2)

struct Result {
    int result;
    string fileName;
    vector<record> recordList;
    vector<string> attributeList;
    vector<int> typeList;
    int operationType;
};

class pStmt {
private:
    int isKey(char *key);
    int lowerCase(char *key, int offset);
    int isLetter(char c);
    int toInteger(const char *str);
    int isString(string s);
public:
    int instructionType;
    vector<Condition> conditionList;
    vector<name> nameList;
    pStmt(string query)
    {
        instructionType = runParser(query);
    }
    ~pStmt();
    int getToken(char *query, int offset, int *tokenTemp, string *tokenList);
    int runParser(string query);
};

class Interpreter {
private:
    BufferManager bm;
    RecordManager *rm;
    IndexManager *im;
    CatalogManager *cm;
    API *api;
public:
    vector<pStmt> pStmtList;
    Interpreter() {
        rm = new RecordManager(bm);
        im = new IndexManager(&bm);
        cm = new CatalogManager(bm);
        api = new API(rm, im, cm);
    }
    ~Interpreter() { 
        pStmtList.clear();
        delete api;
        delete rm;
        delete im;
        delete cm;
    }
    int getPStmt(string query);
    int checkAttribute();
    int runPStmt(vector<Result> *resultList);
    int checkValues(string s, int type);
};
