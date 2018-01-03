#include "IndexManager.h"

BPlusTreeNode::BPlusTreeNode(int keytype)
{
    KeyType = keytype;
    KeySize = KeyType <= 255 ? KeyType : 4;
    ItemSize = sizeof(Address) + KeySize;
    KeyCapacity = (BLOCKSIZE - 5 - sizeof(Address)) / ItemSize;
}

BPlusTreeNode::~BPlusTreeNode()
{ }

BPlusTreeNode& BPlusTreeNode::operator=(const BPlusTreeNode& b)
{
    LeafFlag = b.LeafFlag;
    KeyCount = b.KeyCount;
    NodeData = b.NodeData;
    KeyType = b.KeyType;
    KeySize = b.KeySize;
    ItemSize = b.ItemSize;
    NodeNumber = b.NodeNumber;
    KeyCapacity = b.KeyCapacity;
    return *this;
}

void BPlusTreeNode::LoadFromBlock(const Block& block, int nodenum)
{
    int addrOffset, keyOffset;
    LeafFlag = (bool)block.Data[0];
    KeyCount = *(int*)&block.Data[1];
    NodeNumber = nodenum;
    NodeData.clear();
    addrOffset = 5;
    keyOffset = addrOffset + sizeof(Address);
    for (int i = 0; i < KeyCount; i++)
    {
        NodeData.push_back(BPlusTreeItem(&block.Data[addrOffset], &block.Data[keyOffset], KeySize));
        addrOffset += ItemSize;
        keyOffset += ItemSize;
    }
    NodeData.push_back(BPlusTreeItem(&block.Data[addrOffset], NULL, KeySize));
}

void BPlusTreeNode::InitBlock()
{
    Address addr;
    addr.FileNumber = 0;
    addr.BlockOffset = 0;
    KeyCount = 0;
    LeafFlag = true;
    NodeData.clear();
    NodeData.push_back(BPlusTreeItem(addr, NULL, KeySize));
}

Block BPlusTreeNode::OutputIntoBlock()
{
    Block block;
    ItemList::iterator it;
    int addrOffset, keyOffset;
    block.Data[0] = (char)LeafFlag;
    *(int*)&block.Data[1] = KeyCount;
    addrOffset = 5;
    keyOffset = addrOffset + sizeof(Address);
    for (it = NodeData.begin(); it != NodeData.end(); it++)
    {
        Address addr = it->GetAddr();
        memcpy(&block.Data[addrOffset], &addr, sizeof(Address));
        if (it->GetKey() != NULL)
            memcpy(&block.Data[keyOffset], it->GetKey(), KeySize);
        addrOffset += ItemSize;
        keyOffset += ItemSize;
    }
    return block;
}

bool BPlusTreeNode::isLeaf()
{
    return LeafFlag;
}

bool BPlusTreeNode::isFull()
{
    return KeyCount >= KeyCapacity;
}

bool BPlusTreeNode::isEmpty()
{
    return KeyCount == 0;
}

int BPlusTreeNode::GetCount()
{
    return KeyCount;
}

int BPlusTreeNode::GetCapacity()
{
    return KeyCapacity;
}

Address BPlusTreeNode::Search(const void* key)
{
    ItemList::iterator it;
    for (it = NodeData.begin(); it != NodeData.end(); it++)
    {
        if (LeafFlag)
        {
            if (CompareKey(key, it->GetKey(), KeyType) == 0)
                return it->GetAddr();
        }
        else
        {
            if (CompareKey(key, it->GetKey(), KeyType) < 0)
                return it->GetAddr();
        }
    }
    Address notfound;
    notfound.FileNumber = 0;
    return notfound;
}

void BPlusTreeNode::Insert(Address addr, const void* key)
{
    ItemList::iterator it, now;
    for (it = NodeData.begin(); it != NodeData.end(); it++)
    {
        if (CompareKey(key, it->GetKey(), KeyType) < 0)
        {
            now = NodeData.insert(it, BPlusTreeItem(addr, key, KeySize));
            KeyCount++;
            break;
        }
    }
    if (!LeafFlag)
    {
        Address t1 = now->GetAddr();
        Address t2 = it->GetAddr();
        it->SetAddr(t1);
        now->SetAddr(t2);
    }
}

void BPlusTreeNode::Delete(const void* key)
{
    ItemList::iterator it;
    for (it = NodeData.begin(); it != NodeData.end(); it++)
    {
        if (CompareKey(key, it->GetKey(), KeyType) == 0)
        {
            NodeData.erase(it);
            KeyCount--;
            break;
        }
    }
}

BPlusTreeItem BPlusTreeNode::Split(Address addr, BPlusTreeNode* newnode)
{
    newnode->LeafFlag = LeafFlag;
    newnode->NodeData.clear();
    if (LeafFlag)
    {
        newnode->NodeData.push_front(NodeData.back());
        NodeData.pop_back();
        for (int i = 0; i < KeyCount / 2; i++)
        {
            newnode->NodeData.push_front(NodeData.back());
            NodeData.pop_back();
        }
        NodeData.push_back(BPlusTreeItem(addr, NULL, KeySize));
        newnode->KeyCount = KeyCount / 2;
        KeyCount = KeyCount - newnode->KeyCount;
        return newnode->NodeData.front();
    }
    else
    {
        newnode->NodeData.push_front(NodeData.back());
        NodeData.pop_back();
        for (int i = 0; i < (KeyCount - 1) / 2; i++)
        {
            newnode->NodeData.push_front(NodeData.back());
            NodeData.pop_back();
        }
        BPlusTreeItem result(NodeData.back());
        NodeData.pop_back();
        NodeData.push_back(BPlusTreeItem(result.GetAddr(), NULL, KeySize));
        newnode->KeyCount = (KeyCount - 1) / 2;
        KeyCount = KeyCount - newnode->KeyCount - 1;
        return result;
    }
}

void BPlusTreeNode::MakeRoot(Address addr, BPlusTreeItem item)
{
    LeafFlag = false;
    KeyCount = 1;
    NodeData.clear();
    NodeData.push_front(BPlusTreeItem(item.GetAddr(), NULL, KeySize));
    item.SetAddr(addr);
    NodeData.push_front(item);
}

Address BPlusTreeNode::FindSibling(int filenum, bool* front)
{
    ItemList::iterator it;
    for (it = NodeData.begin(); it != NodeData.end(); it++)
        if (it->GetAddr().FileNumber == filenum)
            break;
    if (it == NodeData.begin())
    {
        it++;
        *front = false;
        return it->GetAddr();
    }
    else
    {
        it--;
        *front = true;
        return it->GetAddr();
    }
}

void BPlusTreeNode::Merge(BPlusTreeNode* node, BPlusTreeNode* sibling, bool front, int* delnum)
{
    ItemList::iterator it;
    *delnum = 0;
    if (node->LeafFlag)
    {
        if (node->KeyCount + sibling->KeyCount > KeyCapacity)
        {
            if (front)
            {
                BPlusTreeItem tempitem = sibling->NodeData.back();
                sibling->NodeData.pop_back();
                node->NodeData.push_front(sibling->NodeData.back());
                sibling->NodeData.pop_back();
                sibling->NodeData.push_back(tempitem);
                for (it = NodeData.begin(); it != NodeData.end(); it++)
                {
                    if (it->GetAddr().FileNumber == sibling->NodeNumber)
                    {
                        it->SetKey(node->NodeData.front().GetKey());
                        break;
                    }
                }
            }
            else
            {
                BPlusTreeItem tempitem = node->NodeData.back();
                node->NodeData.pop_back();
                node->NodeData.push_back(sibling->NodeData.front());
                sibling->NodeData.pop_front();
                node->NodeData.push_back(tempitem);
                for (it = NodeData.begin(); it != NodeData.end(); it++)
                {
                    if (it->GetAddr().FileNumber == node->NodeNumber)
                    {
                        it->SetKey(sibling->NodeData.front().GetKey());
                        break;
                    }
                }
            }
            node->KeyCount++;
            sibling->KeyCount--;
        }
        else
        {
            if (front)
            {
                for (it = NodeData.begin(); it != NodeData.end(); it++)
                {
                    if (it->GetAddr().FileNumber == sibling->NodeNumber)
                    {
                        sibling->NodeData.pop_back();
                        sibling->NodeData.splice(sibling->NodeData.end(), node->NodeData);
                        sibling->KeyCount = sibling->KeyCount + node->KeyCount;
                        *delnum = node->NodeNumber;
                        Address tempaddr = it->GetAddr();
                        NodeData.erase(it++);
                        it->SetAddr(tempaddr);
                        KeyCount--;
                        break;
                    }
                }
            }
            else
            {
                for (it = NodeData.begin(); it != NodeData.end(); it++)
                {
                    if (it->GetAddr().FileNumber == node->NodeNumber)
                    {
                        node->NodeData.pop_back();
                        node->NodeData.splice(node->NodeData.end(), sibling->NodeData);
                        node->KeyCount = node->KeyCount + sibling->KeyCount;
                        *delnum = sibling->NodeNumber;
                        Address tempaddr = it->GetAddr();
                        NodeData.erase(it++);
                        it->SetAddr(tempaddr);
                        KeyCount--;
                        break;
                    }
                }
            }
        }
    }
    else
    {
        if (node->KeyCount + sibling->KeyCount >= KeyCapacity)
        {
            if (front)
            {
                for (it = NodeData.begin(); it != NodeData.end(); it++)
                {
                    if (it->GetAddr().FileNumber == sibling->NodeNumber)
                    {
                        node->NodeData.push_front(sibling->NodeData.back());
                        sibling->NodeData.pop_back();
                        node->NodeData.front().SetKey(it->GetKey());
                        it->SetKey(sibling->NodeData.back().GetKey());
                        sibling->NodeData.back().SetKey(NULL);
                        break;
                    }
                }
            }
            else
            {
                for (it = NodeData.begin(); it != NodeData.end(); it++)
                {
                    if (it->GetAddr().FileNumber == node->NodeNumber)
                    {
                        node->NodeData.back().SetKey(it->GetKey());
                        it->SetKey(sibling->NodeData.front().GetKey());
                        node->NodeData.push_back(sibling->NodeData.front());
                        node->NodeData.back().SetKey(NULL);
                        sibling->NodeData.pop_front();
                        break;
                    }
                }
            }
            node->KeyCount++;
            sibling->KeyCount--;
        }
        else
        {
            if (front)
            {
                for (it = NodeData.begin(); it != NodeData.end(); it++)
                {
                    if (it->GetAddr().FileNumber == sibling->NodeNumber)
                    {
                        sibling->NodeData.back().SetKey(it->GetKey());
                        sibling->NodeData.splice(sibling->NodeData.end(), node->NodeData);
                        sibling->KeyCount = sibling->KeyCount + node->KeyCount + 1;
                        *delnum = node->NodeNumber;
                        Address tempaddr = it->GetAddr();
                        NodeData.erase(it++);
                        it->SetAddr(tempaddr);
                        KeyCount--;
                        break;
                    }
                }
            }
            else
            {
                for (it = NodeData.begin(); it != NodeData.end(); it++)
                {
                    if (it->GetAddr().FileNumber == node->NodeNumber)
                    {
                        node->NodeData.back().SetKey(it->GetKey());
                        node->NodeData.splice(node->NodeData.end(), sibling->NodeData);
                        node->KeyCount = node->KeyCount + sibling->KeyCount + 1;
                        *delnum = sibling->NodeNumber;
                        Address tempaddr = it->GetAddr();
                        NodeData.erase(it++);
                        it->SetAddr(tempaddr);
                        KeyCount--;
                        break;
                    }
                }
            }
        }
    }
}

int BPlusTreeNode::CompareKey(const void* key1, const void* key2, int keytype)
{
    if (key2 == NULL)
        return -1;
    if (keytype <= 255)
        return strncmp((char*)key1, (char*)key2, keytype);
    else if (keytype == 256)
        return *(int*)key1 - *(int*)key2;
    else if (keytype == 257)
    {
        float f1 = *(float*)key1, f2 = *(float*)key2;
        if (f1 - f2 < FLT_EPSILON && f1 - f2 > -FLT_EPSILON)
            return 0;
        else if (f1 - f2 < 0)
            return -1;
        else
            return 1;
    }
}

BPlusTreeItem::BPlusTreeItem(const void* addr, const void* key, int keysize)
{
    Addr = *(Address*)addr;
    KeySize = keysize;
    if (key != NULL)
    {
        Key = new char[KeySize];
        memcpy(Key, key, KeySize);
    }
    else
        Key = NULL;
}

BPlusTreeItem::BPlusTreeItem(Address addr, const void* key, int keysize)
{
    Addr = addr;
    KeySize = keysize;
    if (key != NULL)
    {
        Key = new char[KeySize];
        memcpy(Key, key, KeySize);
    }
    else
        Key = NULL;
}

BPlusTreeItem::BPlusTreeItem(const BPlusTreeItem& b)
{
    Addr = b.Addr;
    KeySize = b.KeySize;
    if (b.Key != NULL)
    {
        Key = new char[KeySize];
        memcpy(Key, b.Key, KeySize);
    }
    else
        Key = NULL;
}

BPlusTreeItem& BPlusTreeItem::operator=(const BPlusTreeItem& b)
{
    Addr = b.Addr;
    KeySize = b.KeySize;
    if (b.Key != NULL)
    {
        if (Key == NULL)
            Key = new char[KeySize];
        memcpy(Key, b.Key, KeySize);
    }
    else if (Key != NULL)
    {
        delete[] Key;
        Key = NULL;
    }
    return *this;
}

BPlusTreeItem::~BPlusTreeItem()
{
    if (Key != NULL)
        delete[] Key;
}

Address BPlusTreeItem::GetAddr()
{
    return Addr;
}

void BPlusTreeItem::SetAddr(Address addr)
{
    Addr = addr;
}

int BPlusTreeItem::GetKeySize()
{
    return KeySize;
}

void* BPlusTreeItem::GetKey()
{
    return Key;
}

void BPlusTreeItem::SetKey(void* key)
{
    if (key != NULL)
    {
        if (Key == NULL)
            Key = new char[KeySize];
        memcpy(Key, key, KeySize);
    }
    else
    {
        delete[] Key;
        Key = NULL;
    }

}

BPlusTree::BPlusTree(int keytype, int root, int nextindex, BufferManager* buffer, const char* indexname)
    : Node(keytype), SiblingNode(keytype), ParentNode(keytype), IndexName(indexname)
{
    KeyType = keytype;
    RootNumber = root;
    NextNewIndexNumber = nextindex;
    Buffer = buffer;
    FileNameOffset = 0;
    memcpy(FileName, indexname, strlen(indexname));
    FileNameOffset += strlen(indexname);
    memcpy(FileName + FileNameOffset, "Index", 5);
    FileNameOffset += 5;
}

BPlusTree::~BPlusTree()
{ }

void BPlusTree::InitRoot()
{
    Buffer->GetBlock(GetFileName(RootNumber));
    Node.InitBlock();
    Buffer->PutBlock(Node.OutputIntoBlock(), GetFileName(RootNumber));
}

Address BPlusTree::Search(const void* key)
{
    Address result;
    Block block = Buffer->GetBlock(GetFileName(RootNumber));
    Node.LoadFromBlock(block, RootNumber);
    Buffer->UnlockBlock(GetFileName(RootNumber));
    result = Node.Search(key);
    while (!Node.isLeaf())
    {
        block = Buffer->GetBlock(GetFileName(result.FileNumber));
        Node.LoadFromBlock(block, result.FileNumber);
        Buffer->UnlockBlock(GetFileName(result.FileNumber));
        result = Node.Search(key);
    }
    return result;
}

void BPlusTree::Insert(Address addr, const void* key)
{
    bool done;
    int nodenum;
    Address result;
    Block block = Buffer->GetBlock(GetFileName(RootNumber));
    Node.LoadFromBlock(block, RootNumber);
    Buffer->UnlockBlock(GetFileName(RootNumber));
    nodenum = RootNumber;
    result.FileNumber = RootNumber;
    while (!Node.isLeaf())
    {
        Path.push(nodenum);
        result = Node.Search(key);
        block = Buffer->GetBlock(GetFileName(result.FileNumber));
        Node.LoadFromBlock(block, result.FileNumber);
        Buffer->UnlockBlock(GetFileName(result.FileNumber));
        nodenum = result.FileNumber;
    }
    if (!Node.isFull())
    {
        Node.Insert(addr, key);
        Buffer->PutBlock(Node.OutputIntoBlock(), GetFileName(result.FileNumber));
        while (!Path.empty())
            Path.pop();
    }
    else
    {
        done = false;
        Address newaddr;
        Node.Insert(addr, key);
        newaddr.FileNumber = NextNewIndexNumber++;
        Buffer->GetBlock(GetFileName(newaddr.FileNumber));
        BPlusTreeItem newitem = Node.Split(newaddr, &SiblingNode);
        Buffer->PutBlock(Node.OutputIntoBlock(), GetFileName(nodenum));
        Buffer->PutBlock(SiblingNode.OutputIntoBlock(), GetFileName(newaddr.FileNumber));
        newitem.SetAddr(newaddr);
        while (!Path.empty())
        {
            block = Buffer->GetBlock(GetFileName(Path.top()));
            Node.LoadFromBlock(block, Path.top());
            if (!Node.isFull())
            {
                Node.Insert(newitem.GetAddr(), newitem.GetKey());
                Buffer->PutBlock(Node.OutputIntoBlock(), GetFileName(Path.top()));
                while (!Path.empty())
                    Path.pop();
                done = true;
                break;
            }
            else
            {
                Node.Insert(newitem.GetAddr(), newitem.GetKey());
                newaddr.FileNumber = NextNewIndexNumber++;
                newitem = Node.Split(newaddr, &SiblingNode);
                Buffer->PutBlock(Node.OutputIntoBlock(), GetFileName(Path.top()));
                Buffer->PutBlock(SiblingNode.OutputIntoBlock(), GetFileName(newaddr.FileNumber));
                newitem.SetAddr(newaddr);
            }
            Path.pop();
        }
        while (!Path.empty())
            Path.pop();
        if (!done)
        {
            Address oldrootaddr;
            oldrootaddr.FileNumber = RootNumber;
            RootNumber = NextNewIndexNumber++;
            block = Buffer->GetBlock(GetFileName(RootNumber));
            Node.MakeRoot(oldrootaddr, newitem);
            Buffer->PutBlock(Node.OutputIntoBlock(), GetFileName(RootNumber));
        }
    }
}

void BPlusTree::Delete(const void * key)
{
    int nodenum;
    Address result;
    Block block = Buffer->GetBlock(GetFileName(RootNumber));
    Node.LoadFromBlock(block, RootNumber);
    Buffer->UnlockBlock(GetFileName(RootNumber));
    nodenum = RootNumber;
    while (!Node.isLeaf())
    {
        Path.push(nodenum);
        result = Node.Search(key);
        block = Buffer->GetBlock(GetFileName(result.FileNumber));
        Node.LoadFromBlock(block, result.FileNumber);
        Buffer->UnlockBlock(GetFileName(result.FileNumber));
        nodenum = result.FileNumber;
    }
    Node.Delete(key);
    if ((Node.GetCount() < (Node.GetCapacity() + 1) / 2) && nodenum != RootNumber)
    {
        bool frontsibling;
        int delnum;
        Address sibling;
        while (!Path.empty())
        {
            block = Buffer->GetBlock(GetFileName(Path.top()));
            ParentNode.LoadFromBlock(block, Path.top());
            sibling = ParentNode.FindSibling(nodenum, &frontsibling);
            block = Buffer->GetBlock(GetFileName(sibling.FileNumber));
            SiblingNode.LoadFromBlock(block, sibling.FileNumber);
            ParentNode.Merge(&Node, &SiblingNode, frontsibling, &delnum);
            if (nodenum == delnum)
                Buffer->Delete(GetFileName(delnum));
            else
                Buffer->PutBlock(Node.OutputIntoBlock(), GetFileName(nodenum));
            if (sibling.FileNumber == delnum)
                Buffer->Delete(GetFileName(delnum));
            else
                Buffer->PutBlock(SiblingNode.OutputIntoBlock(), GetFileName(sibling.FileNumber));
            if (ParentNode.GetCount() >= ParentNode.GetCapacity() / 2)
            {
                Buffer->PutBlock(ParentNode.OutputIntoBlock(), GetFileName(Path.top()));
                break;
            }
            else if (Path.top() == RootNumber)
            {
                if (ParentNode.GetCount() == 0)
                {
                    RootNumber = nodenum == delnum ? sibling.FileNumber : nodenum;
                    Buffer->Delete(GetFileName(Path.top()));
                    if (nodenum == delnum)
                        Buffer->PutBlock(SiblingNode.OutputIntoBlock(), GetFileName(RootNumber));
                    else
                        Buffer->PutBlock(Node.OutputIntoBlock(), GetFileName(RootNumber));
                }
                else
                    Buffer->PutBlock(ParentNode.OutputIntoBlock(), GetFileName(RootNumber));
                break;
            }
            else
            {
                Node = ParentNode;
                nodenum = Path.top();
            }
            Path.pop();
        }
        while (!Path.empty())
            Path.pop();
    }
    else
    {
        Buffer->PutBlock(Node.OutputIntoBlock(), GetFileName(nodenum));
        while (!Path.empty())
            Path.pop();
    }
}

void BPlusTree::Drop()
{
    for (int i = 1; i < NextNewIndexNumber; i++)
        Buffer->Delete(GetFileName(i));
}

char* BPlusTree::GetFileName(int num)
{
    sprintf(FileName + FileNameOffset, "%d.db", num);
    return FileName;
}

IndexManager::IndexManager(BufferManager* buffer)
{
    Buffer = buffer;
    Tree.clear();
    IndexMapTree.clear();
}

IndexManager::~IndexManager()
{
    for (int i = 0; i < Tree.size(); i++)
        if (Tree[i] != NULL)
            delete Tree[i];
}

void IndexManager::CreateIndex(int keytype, const char* indexname)
{
    BPlusTree* tree = new BPlusTree(keytype, 1, 2, Buffer, indexname);
    tree->InitRoot();
    Tree.push_back(tree);
    string word = indexname;
    IndexMapTree.insert(map<string, int>::value_type(word, Tree.size() - 1));
}

void IndexManager::LoadIndex(int keytype, int root, int next, const char* indexname)
{
    BPlusTree* tree = new BPlusTree(keytype, root, next, Buffer, indexname);
    Tree.push_back(tree);
    string word = indexname;
    IndexMapTree.insert(map<string, int>::value_type(word, Tree.size() - 1));
}

void IndexManager::DropIndex(int keytype, int root, int next, const char* indexname)
{
    string word = indexname;
    if (IndexMapTree.find(word) == IndexMapTree.end())
        LoadIndex(keytype, root, next, indexname);
    Tree.at(IndexMapTree.at(word))->Drop();
    delete Tree.at(IndexMapTree.at(word));
    Tree.at(IndexMapTree.at(word)) = NULL;
    IndexMapTree.erase(word);
}

Address IndexManager::Select(const char* indexname, const char* key)
{
    string word = indexname;
    return Tree.at(IndexMapTree.at(word))->Search(key);
}

void IndexManager::Insert(const char* indexname, Address addr, const char* key)
{
    string word = indexname;
    Tree.at(IndexMapTree.at(word))->Insert(addr, key);
}

void IndexManager::Delete(const char* indexname, const char* key)
{
    string word = indexname;
    Tree.at(IndexMapTree.at(word))->Delete(key);
}

bool IndexManager::isLoadedIndex(const char* indexname)
{
    string word = indexname;
    return IndexMapTree.find(word) != IndexMapTree.end();
}

vector<IndexSet> IndexManager::GetAllIndexSet()
{
    vector<IndexSet> result;
    IndexSet set;
    for (int i = 0; i < Tree.size(); i++)
    {
        if (Tree[i] == NULL)
            continue;
        set.name = Tree.at(i)->GetIndexName();
        set.root = Tree.at(i)->GetRootNumber();
        set.next = Tree.at(i)->GetNextNumber();
        result.push_back(set);
    }
    return result;
}
