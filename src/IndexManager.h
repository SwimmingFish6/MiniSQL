#pragma once
#include "DataStructure.h"
#include "BufferManager.h"
#include <list>
#include <stack>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <string.h>
#include <float.h>
using namespace std;

typedef struct
{
    string name;
    int root;
    int next;
} IndexSet;

class BPlusTreeItem
{
public:
    BPlusTreeItem(const void* addr, const void* key, int keysize);
    BPlusTreeItem(Address addr, const void* key, int keysize);
    BPlusTreeItem(const BPlusTreeItem& b);
    ~BPlusTreeItem();
    BPlusTreeItem& operator=(const BPlusTreeItem& b);
    Address GetAddr();
    void SetAddr(Address addr);
    int GetKeySize();
    void* GetKey();
    void SetKey(void* key);
private:
    Address Addr;
    int KeySize;
    void* Key;
};

typedef list<BPlusTreeItem> ItemList;

class BPlusTreeNode
{
public:
    BPlusTreeNode(int keytype);
    ~BPlusTreeNode();
    BPlusTreeNode& operator=(const BPlusTreeNode& b);
    void LoadFromBlock(const Block& block, int nodenum);
    void InitBlock();
    Block OutputIntoBlock();
    bool isLeaf();
    bool isFull();
    bool isEmpty();
    int GetCount();
    int GetCapacity();
    Address Search(const void* key);
    void Insert(Address addr, const void* key);
    void Delete(const void* key);
    BPlusTreeItem Split(Address addr, BPlusTreeNode* newnode);
    void MakeRoot(Address addr, BPlusTreeItem item);
    Address FindSibling(int filenum, bool* front);
    void Merge(BPlusTreeNode* node, BPlusTreeNode* sibling, bool front, int* delnum);
private:
    /*需要存储在DB Files里面的数据*/
    bool LeafFlag;
    int KeyCount;
    ItemList NodeData;
    /*额外的定义信息*/
    int KeyType;
    int KeySize;
    int ItemSize;
    int NodeNumber;
    int KeyCapacity;
    int CompareKey(const void* key1, const void* key2, int keytype);
};

class BPlusTree
{
public:
    BPlusTree(int keytype, int root, int nextindex, BufferManager* buffer, const char* indexname);
    ~BPlusTree();
    void InitRoot();
    Address Search(const void* key);
    void Insert(Address addr, const void* key);
    void Delete(const void* key);
    void Drop();
    string GetIndexName() { return IndexName; }
    int GetRootNumber() { return RootNumber; }
    int GetNextNumber() { return NextNewIndexNumber; }
private:
    int KeyType;
    int RootNumber;
    stack<int> Path;
    int NextNewIndexNumber;
    BufferManager* Buffer;
    BPlusTreeNode Node, SiblingNode, ParentNode;
    string IndexName;
    char FileName[256];
    int FileNameOffset;
    char* GetFileName(int num);
};

class IndexManager
{
public:
    IndexManager(BufferManager* buffer);
    ~IndexManager();
    void CreateIndex(int keytype, const char* indexname);
    void LoadIndex(int keytype, int root, int next, const char* indexname);
    void DropIndex(int keytype, int root, int next, const char* indexname);
    Address Select(const char* indexname, const char* key);
    void Insert(const char* indexname, Address addr, const char* key);
    void Delete(const char* indexname, const char* key);
    bool isLoadedIndex(const char* indexname);
    vector<IndexSet> GetAllIndexSet();
private:
    BufferManager* Buffer;
    vector<BPlusTree*> Tree;
    map<string, int> IndexMapTree;
};
