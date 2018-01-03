#include "Interpreter.h"

#define TK_SELECT     ( 0)
#define TK_FROM       ( 1)
#define TK_WHERE      ( 2)
#define TK_CREATE     ( 3)
#define TK_DROP       ( 4)
#define TK_INSERT     ( 5)
#define TK_INTO       ( 6)
#define TK_VALUES     ( 7)
#define TK_DELETE     ( 8)
#define TK_TABLE      ( 9)
#define TK_INDEX      (10)
#define TK_EXECFILE   (11)
#define TK_QUIT       (12)
#define TK_PRIMARY    (13)
#define TK_KEY        (14)
#define TK_UNIQUE     (15)
#define TK_CHAR       (16)
#define TK_INT        (17)
#define TK_FLOAT      (18)
#define TK_ON         (19)
#define TK_AND        (20)
//#define TK_CONDITION  (21)
#define TK_NAME       (22)
#define TK_ILLEGAL    (23)
#define TK_DIVISION   (24) // ,
#define TK_E          (25) // =
#define TK_NE         (26) // <>
#define TK_S          (27) // <
#define TK_G          (28) // >
#define TK_SE         (29) // <=
#define TK_GE         (30) // >=
#define TK_END        (31) // ;
#define NOTKEY        (32)
#define TK_LEFT       (33) // (
#define TK_RIGHT      (34) // )
#define TK_ALL        (35) // *

#define UPCASE        ( 1)
#define LOWERCASE     (-1)
#define NOTDIGIT      ( 0)

int pStmt::getToken(char *query, int length, int *tokenTemp, string *tokenList)
{
    int i;
    int tokenNumber = 0;
    int key, tokenType;
    char token[100];
    while (' ' == query[0] || '\n' == query[0] || '\r' == query[0] || '\t' == query[0]) {
        query++;
        length--;
    }
    
    while (length > 0) {
        if (' ' == query[0] || '\n' == query[0] || '\r' == query[0] || '\t' == query[0]) {  //跳过所有空格符
            query++;
            length--;
            continue;
        }
        if ('\'' == query[0]) {  //处理‘’字符串
            i = 1;
            while (query[i] != '\'' && length > i)
                i++;
            if (i >= length)
                return TK_ILLEGAL;
            tokenTemp[tokenNumber] = TK_NAME;
            memcpy(token, query, i + 1);
            token[i + 1] = '\0';
            tokenList[tokenNumber] = string(token);
            tokenNumber++;
            length -= i + 1;
            query += i + 1;
            continue;
        }
        i = 0;
        while (isLetter(query[i]) || '0' <= query[i] && '9' >= query[i] || '-' == query[i] || '.' == query[i]) {
            //关键字和名字的判断
            i++;
        }
        if (i != 0) {

            memcpy(token, query, i);
            token[i] = '\0';
            tokenType = isKey(token);
            if (NOTKEY != tokenType) {
                tokenTemp[tokenNumber] = tokenType;
                tokenList[tokenNumber] = string(token);
            }
            else {
                tokenTemp[tokenNumber] = TK_NAME;
                tokenList[tokenNumber] = string(token);
            }
            length -= i;
            query += i;
            tokenNumber++;
        }
        else {
            if (',' == query[0]) {
                tokenType = TK_DIVISION;
                tokenTemp[tokenNumber] = tokenType;
                memcpy(token, query, 1);
                token[1] = '\0';
                tokenList[tokenNumber] = string(token);
                tokenNumber++;
                length--;
                query++;
            }
            else if ('>' == query[0] && '=' == query[1]) {
                tokenType = TK_GE;
                tokenTemp[tokenNumber] = tokenType;
                memcpy(token, query, 2);
                token[2] = '\0';
                tokenList[tokenNumber] = string(token);
                tokenNumber++;
                length -= 2;
                query += 2;
            }
            else if ('<' == query[0] && '=' == query[1]) {
                tokenType = TK_SE;
                tokenTemp[tokenNumber] = tokenType;
                memcpy(token, query, 2);
                token[2] = '\0';
                tokenList[tokenNumber] = string(token);
                tokenNumber++;
                length -= 2;
                query += 2;
            }
            else if ('<' == query[0] && '>' == query[1]) {
                tokenType = TK_NE;
                tokenTemp[tokenNumber] = tokenType;
                memcpy(token, query, 2);
                token[2] = '\0';
                tokenList[tokenNumber] = string(token);
                tokenNumber++;
                length -= 2;
                query += 2;
            }

            else if ('=' == query[0]) {
                tokenType = TK_E;
                tokenTemp[tokenNumber] = tokenType;
                memcpy(token, query, 1);
                token[1] = '\0';
                tokenList[tokenNumber] = string(token);
                tokenNumber++;
                length--;
                query++;
            }
            else if ('>' == query[0]) {
                tokenType = TK_G;
                tokenTemp[tokenNumber] = tokenType;
                memcpy(token, query, 1);
                token[1] = '\0';
                tokenList[tokenNumber] = string(token);
                tokenNumber++;
                length--;
                query++;
            }
            else if ('<' == query[0]) {
                tokenType = TK_S;
                tokenTemp[tokenNumber] = tokenType;
                memcpy(token, query, 1);
                token[1] = '\0';
                tokenList[tokenNumber] = string(token);
                tokenNumber++;
                length--;
                query++;
            }
            else if ('(' == query[0]) {
                tokenType = TK_LEFT;
                tokenTemp[tokenNumber] = tokenType;
                memcpy(token, query, 1);
                token[1] = '\0';
                tokenList[tokenNumber] = string(token);
                tokenNumber++;
                length--;
                query++;
            }
            else if (')' == query[0]) {
                tokenType = TK_RIGHT;
                tokenTemp[tokenNumber] = tokenType;
                memcpy(token, query, 1);
                token[1] = '\0';
                tokenList[tokenNumber] = string(token);
                tokenNumber++;
                length--;
                query++;
            }
            else if ('*' == query[0]) {
                tokenType = TK_ALL;
                tokenTemp[tokenNumber] = tokenType;
                memcpy(token, query, 1);
                token[1] = '\0';
                tokenList[tokenNumber] = string(token);
                tokenNumber++;
                length--;
                query++;
            }
        }
    }
    if (0 != tokenNumber)
        return tokenTemp[0];
    else return TK_ILLEGAL;
}

int pStmt::isKey(char *key)
{
    if (!strcmp(key, "select") || !strcmp(key, "SELECT"))
        return TK_SELECT;
    else if(!strcmp(key, "from") || !strcmp(key, "FROM"))
        return TK_FROM;
    else if (!strcmp(key, "where") || !strcmp(key, "WHERE"))
        return TK_WHERE;
    else if (!strcmp(key, "create") || !strcmp(key, "CREATE"))
        return TK_CREATE;
    else if (!strcmp(key, "drop") || !strcmp(key, "DROP"))
        return TK_DROP;
    else if (!strcmp(key, "insert") || !strcmp(key, "INSERT"))
        return TK_INSERT;
    else if (!strcmp(key, "into") || !strcmp(key, "INTO"))
        return TK_INTO;
    else if (!strcmp(key, "values") || !strcmp(key, "VALUES"))
        return TK_VALUES;
    else if (!strcmp(key, "delete") || !strcmp(key, "DELETE"))
        return TK_DELETE;
    else if (!strcmp(key, "index") || !strcmp(key, "INDEX"))
        return TK_INDEX;
    else if (!strcmp(key, "table") || !strcmp(key, "TABLE"))
        return TK_TABLE;
    else if (!strcmp(key, "execfile") || !strcmp(key, "EXECFILE"))
        return TK_EXECFILE;
    else if (!strcmp(key, "quit") || !strcmp(key, "QUIT"))
        return TK_QUIT;
    else if (!strcmp(key, "primary") || !strcmp(key, "PRIMARY"))
        return TK_PRIMARY;
    else if (!strcmp(key, "key") || !strcmp(key, "KEY"))
        return TK_KEY;
    else if (!strcmp(key, "unique") || !strcmp(key, "UNIQUE"))
        return TK_UNIQUE;
    else if (!strcmp(key, "char") || !strcmp(key, "CHAR"))
        return TK_CHAR;
    else if (!strcmp(key, "int") || !strcmp(key, "INT"))
        return TK_INT;
    else if (!strcmp(key, "float") || !strcmp(key, "FLOAT"))
        return TK_FLOAT;
    else if (!strcmp(key, "on") || !strcmp(key, "ON"))
        return TK_ON;
    else if (!strcmp(key, "and") || !strcmp(key, "AND"))
        return TK_AND;
    else return NOTKEY;
}

int pStmt::lowerCase(char *key, int offset)
{
    int flag;
    if (NULL == key || offset <= 0)
        return -1;
    flag = isLetter(key[0]);
    for (int i = 1; i < 0; i++) {
        if (UPCASE == isLetter(key[i])) {  //关键字不允许大小写混用
            key[i] = key[i] + 'a' - 'A';
            if (LOWERCASE == flag)
                return -1;
        }
        else if (NOTDIGIT == isLetter(key[i]))
            return -1;
        else {
            if (UPCASE == flag)
                return -1;
        }
    }
}

int pStmt::isLetter(char c)
{
    if ('a' <= c && 'z' >= c)
        return LOWERCASE;
    else if ('A' <= c && 'Z' >= c)
        return UPCASE;
    else return NOTDIGIT;
}

int pStmt::runParser(string query)
{
    int i;
    int length = query.length();
    char *statement;
    int tokenTemp[100];
    int values_int, count;
    string tokenList[100];
    name *nameTemp;
    Condition *conditionTemp;
    statement = (char *)malloc(length + 1);
    strcpy(statement, query.c_str());
    
    for (i = 0; i < 100; i++)
        tokenTemp[i] = -1;
    instructionType = getToken(statement, length, tokenTemp, tokenList);
    if (TK_ILLEGAL == instructionType) {
        instructionType = ILLEGAL;
        return ILLEGAL;
    }
    count = 0;
    switch (instructionType) {
    case TK_CREATE:                                                          //create
        if (TK_TABLE == tokenTemp[1]) {                                      //table
            instructionType = CREATE_TABLE;                                  
            if (TK_NAME != tokenTemp[2])                                     //name
                return ILLEGAL;
            nameTemp = new name();
            nameTemp->word = string(tokenList[2]);
            nameTemp->value1 = 0;
            nameList.push_back(*nameTemp);
            delete nameTemp;
            if (TK_LEFT != tokenTemp[3])                                     //(
                return ILLEGAL;
            i = 4;
            while (1) {
                if (TK_NAME != tokenTemp[i]) {                               //name
                    if (TK_PRIMARY != tokenTemp[i]                           //primary key (name)
                        || TK_KEY != tokenTemp[i + 1]
                        || TK_LEFT != tokenTemp[i + 2]
                        || TK_NAME != tokenTemp[i + 3]
                        || TK_RIGHT != tokenTemp[i + 4])
                        return ILLEGAL;
                    else {
                        count++;
                        nameTemp = new name();
                        nameTemp->word = string(tokenList[i + 3]);
                        nameTemp->value1 = 2;
                        i += 5;
                        nameList.push_back(*nameTemp);
                        delete nameTemp;
                    }
                }
                else {                                                       //name
                    nameTemp = new name();
                    nameTemp->word = string(tokenList[i]);
                    nameTemp->value1 = 1;
                    i++;

                    if (TK_INT == tokenTemp[i]) {                            //int
                        nameTemp->value2 = 256;
                        i++;
                    }
                    else if (TK_FLOAT == tokenTemp[i]) {                     //int
                        nameTemp->value2 = 257;
                        i++;
                    }
                    else if (TK_CHAR == tokenTemp[i]) {                      //char
                        if (TK_LEFT != tokenTemp[i + 1]                      //(n)
                            || !(values_int = toInteger(tokenList[i + 2].c_str()))
                            || TK_RIGHT != tokenTemp[i + 3])
                            return ILLEGAL;
                        nameTemp->value2 = values_int;
                        i += 4;
                    }
                    else return ILLEGAL;
                    if (TK_UNIQUE == tokenTemp[i]) {                         //unique
                        nameTemp->value3 = 1;
                        i++;
                    }
                    else nameTemp->value3 = 0;
                    nameList.push_back(*nameTemp);
                    delete nameTemp;
                }
                if (TK_DIVISION != tokenTemp[i] && TK_RIGHT != tokenTemp[i])
                    return ILLEGAL;
                if (TK_DIVISION == tokenTemp[i])                             //,
                    i++;
                else if (TK_RIGHT == tokenTemp[i]) {                         //)
                    i++;
                    break;
                }
            }
            if (1 != count)
                return ILLEGAL;
            if (-1 != tokenTemp[i])
                return ILLEGAL;
        }
        else if (TK_INDEX == tokenTemp[1]) {                                 //index
            instructionType = CREATE_INDEX;
            if (TK_NAME != tokenTemp[2]                                      //name
                || TK_ON != tokenTemp[3]                                     //on
                || TK_NAME != tokenTemp[4]                                   //name
                || TK_LEFT != tokenTemp[5]                                   //(
                || TK_NAME != tokenTemp[6]                                   //name
                || TK_RIGHT !=tokenTemp[7]                                   //)
                || -1 != tokenTemp[8])
                return ILLEGAL;
            nameTemp = new name();
            nameTemp->word = tokenList[2];
            nameTemp->value1 = 0;
            nameList.push_back(*nameTemp);
            delete nameTemp;
            nameTemp = new name();
            nameTemp->word = tokenList[4];
            nameTemp->value1 = 1;
            nameList.push_back(*nameTemp);
            delete nameTemp;
            nameTemp = new name();
            nameTemp->word = tokenList[6];
            nameTemp->value1 = 2;
            nameList.push_back(*nameTemp);
            delete nameTemp;
        }
        else instructionType = ILLEGAL;
        break;
    case TK_SELECT:                                                             //select
        instructionType = SELECT;
        i = 1;
        while (1) {
            if (TK_ALL == tokenTemp[1]) {                                       //*
                nameTemp = new name();
                nameTemp->word = "*";
                nameTemp->value1 = 0;
                nameList.push_back(*nameTemp);
                delete nameTemp;
                i++;
                break;
            }
            else {
                if (TK_NAME == tokenTemp[i]) {                                  //name
                    nameTemp = new name();
                    nameTemp->word = tokenList[i];
                    nameTemp->value1 = 0;
                    nameList.push_back(*nameTemp);
                    delete nameTemp;
                    i++;
                }
                else return ILLEGAL;
                if (TK_DIVISION != tokenTemp[i])                                //,
                    break;
                else i++;
            }
        }
        if (TK_FROM != tokenTemp[i]                                             //from name
            || TK_NAME != tokenTemp[i + 1])
            return ILLEGAL;
        else {
            nameTemp = new name();
            nameTemp->word = tokenList[i + 1];
            nameTemp->value1 = 1;
            nameList.push_back(*nameTemp);
            delete nameTemp;
            i += 2;
        }
        if (-1 == tokenTemp[i])
            break;
        else {
            if (TK_WHERE != tokenTemp[i])                                      //where
                return ILLEGAL;
            i++;
            while (-1 != tokenTemp[i]) {
                if (TK_NAME != tokenTemp[i]                                    //name op name
                    || TK_E > tokenTemp[i + 1]
                    || TK_GE < tokenTemp[i + 1]
                    || TK_NAME != tokenTemp[i + 2])
                    return ILLEGAL;
                conditionTemp = new Condition(tokenList[i], tokenList[i + 2], tokenTemp[i + 1] - TK_E);
                conditionList.push_back(*conditionTemp);
                delete conditionTemp;
                i += 3;
                if (TK_AND != tokenTemp[i])                                    //and
                    break;
                else i++;
            }
            if (-1 != tokenTemp[i])
                return ILLEGAL;
        }
        break; 
    case TK_INSERT:                                                            //insert
        instructionType = INSERT;
        if (TK_INTO != tokenTemp[1]                                            //into
            || TK_NAME != tokenTemp[2]                                         //name
            || TK_VALUES != tokenTemp[3])                                      //values
            return ILLEGAL;
        nameTemp = new name();
        nameTemp->word = tokenList[2];
        nameTemp->value1 = 0;
        nameList.push_back(*nameTemp);
        delete nameTemp;
        i = 4;
        if (TK_LEFT != tokenTemp[i])                                           //(
            return ILLEGAL;
        else {
            i++;
            while (1) {
                if (TK_NAME != tokenTemp[i])                                   //name
                    return ILLEGAL;
                nameTemp = new name();
                nameTemp->word = tokenList[i];
                nameTemp->value1 = 1;
                //nameTemp->value2 = isString(tokenList[i]);
                //if (-1 == nameTemp->value2)
                //    return ILLEGAL;
                //isString()
                nameList.push_back(*nameTemp);
                delete nameTemp;
                i++;
                if (TK_DIVISION != tokenTemp[i] && TK_RIGHT != tokenTemp[i])
                    return ILLEGAL;
                if (TK_DIVISION == tokenTemp[i])                               //,
                    i++;                                                       
                else if (TK_RIGHT == tokenTemp[i]) {                           //)
                    i++;
                    break;
                }
            }
            if (-1 != tokenTemp[i])
                return ILLEGAL;
        }
        break;
    case TK_DELETE:                                                            //delete
        instructionType = DELETE;
        if (TK_FROM != tokenTemp[1]                                            //from
            || TK_NAME != tokenTemp[2])                                        //name
            return ILLEGAL;
        nameTemp = new name();
        nameTemp->word = tokenList[2];
        nameTemp->value1 = 0;
        nameList.push_back(*nameTemp);
        delete nameTemp;
        i = 3;
        if (-1 == tokenTemp[i])
            break;
        else {
            if (TK_WHERE != tokenTemp[i])                                      //where
                return ILLEGAL;
            i++;
            while (-1 != tokenTemp[i]) {
                if (TK_NAME != tokenTemp[i]                                    //name op name
                    || TK_E > tokenTemp[i + 1]
                    || TK_GE < tokenTemp[i + 1]
                    || TK_NAME != tokenTemp[i + 2])
                    return ILLEGAL;
                conditionTemp = new Condition(tokenList[i], tokenList[i + 2], tokenTemp[i + 1] - TK_E);
                conditionList.push_back(*conditionTemp);
                delete conditionTemp;
                i += 3;
                if (TK_AND != tokenTemp[i])                                    //and
                    break;
                else i++;
            }
            if (-1 != tokenTemp[i])
                return ILLEGAL;
        }
        break;
    case TK_DROP:
        if (TK_TABLE == tokenTemp[1]) {
            instructionType = DROP_TABLE;
        }
        else if(TK_INDEX == tokenTemp[1]){
            instructionType = DROP_INDEX;
        }
        else return ILLEGAL;
        if (TK_NAME != tokenTemp[2]
            || -1 != tokenTemp[3])
            return ILLEGAL;
        nameTemp = new name();
        nameTemp->word = tokenList[2];
        nameTemp->value1 = 0;
        nameList.push_back(*nameTemp);
        delete nameTemp;
        break;
    case TK_QUIT:
        instructionType = QUIT;
        if (-1 != tokenTemp[1])
            return ILLEGAL;
        break;
    case TK_EXECFILE:
        instructionType = EXECFILE;
        if (TK_NAME != tokenTemp[1]
            || -1 != tokenTemp[2])
            return ILLEGAL;
        nameTemp = new name();
        nameTemp->word = tokenList[1];
        nameTemp->value1 = 0;
        nameList.push_back(*nameTemp);
        delete nameTemp;
        break;
    default:
        instructionType = ILLEGAL;
    }

    return instructionType;
}

int pStmt::toInteger(const char *str)
{
    int i, flag, output;
    i = 0;
    while ('\0' != str[i]) {
        if ('0' > str[i] || '9' < str[i])
            return 0;
        i++;
    }
    output = atoi(str);

    return output;
}

int pStmt::isString(string s)
{
    int i, flag, count;
    if ('\'' == s[0] && '\'' == s[s.length() - 1])
        return 1;
    i = 0;
    if ('-' == s[i])
        i++;
    count = 0;
    if ('.' == s[s.length() - 1]) {
        return -1;
    }
    while (i < s.length()) {
        if ('.' == s[i]) {
            count++;
            if (count > 1)
                return -1;
        }
        else if ('0' > s[i] || '9' < s[i]) {
            return -1;
        }
        i++;
    }
    return 0;
}

int Interpreter::getPStmt(string query)
{
    pStmtList.clear();
    char str[1000], strTemp[1000];
    strcpy(str, query.c_str());
    int length, i;
    pStmt *pStmtTemp;
    length = query.length();
    i = 0;
    char * pstr = str;
    while (i < length) {
        if (';' == pstr[i]) {
            memcpy(strTemp, pstr, i + 1);
            strTemp[i] = '\0';
            pStmtTemp = new pStmt(string(strTemp));
            pStmtList.push_back(*pStmtTemp);
            pstr += i + 1;
            length -= i + 1;
            i = 0;
            delete pStmtTemp;
        }
        i++;
    }
    checkAttribute(); //对属性进行检查
    return 0;
}

int Interpreter::runPStmt(vector<Result> *resultList)
{
    vector<pStmt>::iterator iter;
    vector<Attribute> attributeList;
    int result;
    Result *resultTemp;
    string tableName;
    int i, j;
    for (iter = pStmtList.begin(); iter != pStmtList.end(); iter++) {
        switch (iter->instructionType) {
            case CREATE_TABLE :
                result = api->CreateTable(iter->nameList);
                resultTemp = new Result();
                resultTemp->result = result;
                resultTemp->operationType = CREATE_TABLE;
                resultList->push_back(*resultTemp);
                break;
            case CREATE_INDEX :
                result = api->CreateIndex(iter->nameList);
                resultTemp = new Result();
                resultTemp->result = result;
                resultTemp->operationType = CREATE_INDEX;
                resultList->push_back(*resultTemp);
                break;
            case SELECT       :
                resultTemp = new Result();
                result = api->Select(iter->nameList, iter->conditionList, resultTemp->recordList);
                resultTemp->result = result;
                resultTemp->operationType = SELECT;
                if (iter->nameList[0].word == "*") {
                    attributeList.clear();
                    cm->getAllAttritube(iter->nameList[1].word.c_str(), &attributeList);
                    for (i = 0; i < attributeList.size(); i++) {
                        resultTemp->attributeList.push_back(string(attributeList[i].getName()));
                        resultTemp->typeList.push_back(attributeList[i].getType());
                    }
                }
                else {

                    for (i = 0; i < iter->nameList.size(); i++) {
                        if (iter->nameList[i].value1 == 0) {
                            resultTemp->attributeList.push_back(iter->nameList[i].word);
                            for (j = 0; j < iter->nameList.size();j++) {
                                if (1 == iter->nameList[j].value1)
                                    tableName = iter->nameList[j].word;
                            }
                            resultTemp->typeList.push_back(cm->getType(tableName.c_str(), iter->nameList[i].word.c_str()));
                        }
                        else break;
                    }
                }
                resultList->push_back(*resultTemp);
                break;
            case INSERT       :
                result = api->Insert(iter->nameList);
                resultTemp = new Result();
                resultTemp->result = result;
                resultTemp->operationType = INSERT;
                resultList->push_back(*resultTemp);
                break;
            case DELETE       :
                result = api->Delete(iter->nameList, iter->conditionList);
                resultTemp = new Result();
                resultTemp->result = result;
                resultTemp->operationType = DELETE;
                resultList->push_back(*resultTemp);
                break;
            case DROP_TABLE   :
                result = api->DropTable(iter->nameList);
                resultTemp = new Result();
                resultTemp->result = result;
                resultTemp->operationType = DROP_TABLE;
                resultList->push_back(*resultTemp);
                break;
            case DROP_INDEX   :
                result = api->DropIndex(iter->nameList);
                resultTemp = new Result();
                resultTemp->result = result;
                resultTemp->operationType = DROP_INDEX;
                resultList->push_back(*resultTemp);
                break;
            case EXECFILE     :
                resultTemp = new Result();
                resultTemp->result = SUCCESS;
                resultTemp->operationType = EXECFILE;
                resultTemp->fileName = iter->nameList[0].word;
                resultList->push_back(*resultTemp);
                break;
            case QUIT         :
                api->CatalogDone();
                resultTemp = new Result();
                resultTemp->result = SUCCESS;
                resultTemp->operationType = QUIT;
                resultList->push_back(*resultTemp);
                break;
            default           : 
                resultTemp = new Result();
                resultTemp->result = iter->instructionType;
                resultList->push_back(*resultTemp);
                break;
        }
        delete resultTemp;
    }
    return pStmtList.size();
}

int Interpreter::checkAttribute()
{
    vector<pStmt>::iterator iterPStmt;
    //对属性进行查找并且对type和isUnique进行设置
    vector<name>::iterator iterName;
    vector<Condition>::iterator iterCondition;
    string tableName;
    string primaryKey;
    int attributeNumber; //>=0表示对对应value1的name和condition进行判断
                         //-1表示不进行判断
                         //insert进行额外考虑
    int count, type, flag, i;
    char attribute[32];
    for (iterPStmt = pStmtList.begin(); iterPStmt != pStmtList.end(); iterPStmt++) {
        
        switch (iterPStmt->instructionType) {
        case CREATE_TABLE:
            flag = 0;
            for (iterName = iterPStmt->nameList.begin(); iterName != iterPStmt->nameList.end(); iterName++) {
                if (2 == iterName->value1) {
                    primaryKey = iterName->word;
                    iterName->value1 = 4;
                }   
            }
            for (iterName = iterPStmt->nameList.begin(); iterName != iterPStmt->nameList.end(); iterName++) {
                if (1 == iterName->value1 && iterName->word == primaryKey) {
                    flag = 1;
                    iterName->value1 = 2;
                    iterName->value3 = 1;
                }
            }
            break;
        case CREATE_INDEX:
            tableName = iterPStmt->nameList[1].word;
            attributeNumber = 2;
            if (!cm->findTable(tableName.c_str()))
                iterPStmt->instructionType = ATTRIBUTEERROR;
            else {
                for (iterName = iterPStmt->nameList.begin(); iterName != iterPStmt->nameList.end(); iterName++) {
                    if (attributeNumber == iterName->value1) {
                        iterName->value2 = cm->getType(tableName.c_str(), iterName->word.c_str());
                        iterName->value3 = cm->getIsUnique(tableName.c_str(), iterName->word.c_str());
                        if (TYPE_ERROR == iterName->value2 || 3 == iterName->value3) {
                            iterPStmt->instructionType = ATTRIBUTEERROR;
                            break;
                        }
                    }
                }
            }
            break;
        case SELECT: 
            count = 0;
            flag = 0;
            for (iterName = iterPStmt->nameList.begin(); iterName != iterPStmt->nameList.end(); iterName++) {
                if (1 == iterName->value1)
                    tableName = iterName->word;
                else if (0 == iterName->value1) {
                    count++;
                    if ("*" == iterName->word)    //对*进行额外的判断
                        flag = 1;
                    if (flag && count > 1) {
                        iterPStmt->instructionType = ATTRIBUTEERROR;
                        break;
                    }
                }
            }
            attributeNumber = 0;
            if (iterPStmt->instructionType != ATTRIBUTEERROR) {
                if (!cm->findTable(tableName.c_str()))
                    iterPStmt->instructionType = ATTRIBUTEERROR;
                else if (!flag) {
                    for (iterName = iterPStmt->nameList.begin(); iterName != iterPStmt->nameList.end(); iterName++) {
                        if (attributeNumber == iterName->value1) {
                            iterName->value2 = cm->getType(tableName.c_str(), iterName->word.c_str());
                            iterName->value3 = cm->getIsUnique(tableName.c_str(), iterName->word.c_str());
                            if (TYPE_ERROR == iterName->value2 || 3 == iterName->value3) {
                                iterPStmt->instructionType = ATTRIBUTEERROR;
                                break;
                            }
                        }
                    }
                }
            }
            break;
        case INSERT:
            tableName = iterPStmt->nameList[0].word;
            if (!cm->findTable(tableName.c_str()))
                iterPStmt->instructionType = ATTRIBUTEERROR;
            else {
                count = cm->getAttributeSize(tableName.c_str());
                if (iterPStmt->nameList.size() != count + 1) {
                    iterPStmt->instructionType = ATTRIBUTEERROR;
                }
                else {
                    i = 0;
                    for (iterName = iterPStmt->nameList.begin() + 1; iterName != iterPStmt->nameList.end(); iterName++) {
                        cm->getAttribute(attribute, tableName.c_str(), i);
                        i++;
                        type = cm->getType(tableName.c_str(), attribute);
                        if (0 == checkValues(iterName->word, type)) {
                            iterPStmt->instructionType = ATTRIBUTEERROR;
                            break;
                        }
                        else if (1 <= type && 255 >= type) {
                            iterName->word = iterName->word.substr(1, iterName->word.length() - 2);//去掉‘’
                        }
                        iterName->value2 = type;
                    }
                }
            }
            break;
        case DELETE: //只有condition部分有属性
            tableName = iterPStmt->nameList[0].word;
            attributeNumber = -1;
            break;
        case DROP_TABLE:
        case DROP_INDEX:
        case EXECFILE:
        case QUIT:
            attributeNumber = -1;
            break;
        }
    }
    return 0;
}

int Interpreter::checkValues(string s, int type)
{
    int i = 0, flag = 0;
    if (256 == type) {
        if ('-' == s[0]) {
            i++;
            if (1 == s.length())
                return 0;
        } 
        while (i < s.length()) {
            if ('0' > s[i] || '9' < s[i])
                return 0;
            i++;
        }
    }
    else if(257 == type) {
        if ('.' == s[0])
            return 0;
        if ('-' == s[0]) {
            i++;
            if ('.' == s[i])
                return 0;
            if (1 == s.length())
                return 0;
        }
        while (i < s.length()) {
            if ('.' == s[i]) {
                if (flag || s.length() - 1 == i)
                    return 0;
                flag = 1;
            }
            else if ('0' > s[i] || '9' < s[i])
                return 0;
            i++;
        }
    }
    else {
        if ('\'' == s[0] && '\'' == s[0] && type == s.length() - 2)
            return type;
        else return 0;
    }
    return type;
}

pStmt::~pStmt()
{
    conditionList.clear();
    nameList.clear();
}
