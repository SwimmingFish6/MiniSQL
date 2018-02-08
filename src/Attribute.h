#pragma once
#include <iostream>
#include <string.h>
#define TYPE_INT (256)
#define TYPE_FLOAT (257)
#define TYPE_ERROR (0)

using namespace std;

class Attribute {
private :
    char name[32];
    int type;
    int isUnique;
    int indexNumber;
    int nextIndexNumber;
    char indexName[32];
public :
    int getType() { return type; }
    int ifUnique() { return isUnique; }
    bool ifIndex() { 
        if (indexNumber < 0)
            return false;
        else return true;
    }
    char *getName() { return name; }
    int getIndexNumber() { return indexNumber; }
    int getNextIndexNumber() { return nextIndexNumber; }
    Attribute(char *name, int type, int isUnique, int indexNumber, int nextIndexNumber, char *indexName)
        :type(type), isUnique(isUnique), indexNumber(indexNumber), nextIndexNumber(nextIndexNumber)
    {
        memcpy(this->name, name, 32);
        memcpy(this->indexName, indexName, 32);
    }
    Attribute(char *name, int type, int isUnique) :type(type), isUnique(isUnique) 
    {
        memcpy(this->name, name, 32);
        indexNumber = -1;
        nextIndexNumber = -1;
    }
    void setIndex(int indexNumber, int nextIndexNumber)
    {
        this->indexNumber = indexNumber;
        this->nextIndexNumber = nextIndexNumber;
    }
    void setIndexName(char *indexName)
    {
        memcpy(this->indexName, indexName, 32);
    }
};
