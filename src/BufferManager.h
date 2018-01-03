#pragma once
#include "DataStructure.h"
#include <fstream>
#include <string>
#include <ctime>
using namespace std;

class BufferBlock
{
public:
    BufferBlock();
    ~BufferBlock();
    Block GetData();
    void SetData(Block data);
    string GetFileName();
    void SetFileName(string filename);
    bool isDirty();
    void SetDirty(bool dirty);
    bool isPin();
    void SetPin(bool pin);
    int GetTimes();
    void AddTimes();
    void ReadBlockFromDisk();
    void WriteBlockToDisk();
private:
    Block DataBlock;
    string Filename;
    bool Dirty;
    bool Pin;
    int Times;
};

class BufferManager
{
public:
    BufferManager();
    ~BufferManager();
    Block GetBlock(const char* blockname);
    void PutBlock(Block data, const char* blockname);
    void UnlockBlock(const char* blockname);
    void Delete(const char* blockname);
    bool FindBlock(const char* blockname);
private:
    BufferBlock* block[NUMOFBLOCK];
    int BlockCount;
    int LFU();
};