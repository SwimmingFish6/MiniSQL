#include "BufferManager.h"

BufferManager::BufferManager()
{
    for (int i = 0; i < NUMOFBLOCK; i++)
        block[i] = new BufferBlock;
    BlockCount = 0;
}

BufferManager::~BufferManager()
{
    for (int i = 0; i < BlockCount; i++)
    {
        block[i]->WriteBlockToDisk();
        delete block[i];
    }
    for (int i = BlockCount; i < NUMOFBLOCK; i++)
        delete block[i];
}

Block BufferManager::GetBlock(const char* blockname)
{
    string name(blockname);
    for (int i = 0; i < BlockCount; i++)
        if (block[i]->GetFileName() == name)
        {
            block[i]->SetPin(true);
            block[i]->AddTimes();
            return block[i]->GetData();
        }
    if (BlockCount < NUMOFBLOCK)
    {
        block[BlockCount]->SetFileName(name);
        block[BlockCount]->ReadBlockFromDisk();
        block[BlockCount]->SetPin(true);
        block[BlockCount]->AddTimes();
        return block[BlockCount++]->GetData();
    }
    else
    {
        int replace = LFU();
        block[replace]->WriteBlockToDisk();
        block[replace]->SetFileName(name);
        block[replace]->ReadBlockFromDisk();
        block[replace]->SetPin(true);
        block[replace]->AddTimes();
        return block[replace]->GetData();
    }
}

void BufferManager::PutBlock(Block data, const char* blockname)
{
    string name(blockname);
    for (int i = 0; i < BlockCount; i++)
        if (block[i]->GetFileName() == name)
        {
            block[i]->SetData(data);
            block[i]->SetDirty(true);
            block[i]->SetPin(false);
            return;
        }
    if (BlockCount < NUMOFBLOCK)
    {
        block[BlockCount]->SetData(data);
        block[BlockCount]->SetFileName(name);
        block[BlockCount]->SetDirty(true);
        block[BlockCount]->SetPin(false);
        BlockCount++;
        return;
    }
    else
    {
        int replace = LFU();
        block[replace]->WriteBlockToDisk();
        block[replace]->SetData(data);
        block[replace]->SetFileName(name);
        block[replace]->SetDirty(true);
        block[replace]->SetPin(false);
        return;
    }
}

void BufferManager::UnlockBlock(const char* blockname)
{
    string name(blockname);
    for (int i = 0; i < BlockCount; i++)
        if (block[i]->GetFileName() == name)
        {
            block[i]->SetPin(false);
            return;
        }
}

void BufferManager::Delete(const char* blockname)
{
    remove(blockname);
    string name(blockname);
    int delindex = -1;
    for (int i = 0; i < BlockCount; i++)
        if (block[i]->GetFileName() == name)
        {
            delindex = i;
            break;
        }
    if (delindex != -1)
    {
        BufferBlock* temp = block[delindex];
        BlockCount--;
        for (int i = delindex; i < BlockCount; i++)
            block[i] = block[i + 1];
        block[BlockCount] = temp;
    }
}

bool BufferManager::FindBlock(const char* blockname)
{
    for (int i = 0; i < BlockCount; i++)
        if (block[i]->GetFileName() == blockname)
            return true;
    FILE* fp;
    fp = fopen(blockname, "rb");
    if (fp != NULL)
    {
        fclose(fp);
        return true;
    }
    return false;
}

int BufferManager::LFU()
{
    int min = INT_MAX, minindex = 0;
    for (int i = 0; i < BlockCount; i++)
        if (block[i]->GetTimes() < min && !block[i]->isPin())
        {
            min = block[i]->GetTimes();
            minindex = i;
        }
    return minindex;
}

BufferBlock::BufferBlock()
{ }

BufferBlock::~BufferBlock()
{ }

Block BufferBlock::GetData()
{
    return DataBlock;
}

void BufferBlock::SetData(Block data)
{
    DataBlock = data;
}

string BufferBlock::GetFileName()
{
    return Filename;
}

void BufferBlock::SetFileName(string filename)
{
    Filename = filename;
}

bool BufferBlock::isDirty()
{
    return Dirty;
}

void BufferBlock::SetDirty(bool dirty)
{
    Dirty = dirty;
}

bool BufferBlock::isPin()
{
    return Pin;
}

void BufferBlock::SetPin(bool pin)
{
    Pin = pin;
}

int BufferBlock::GetTimes()
{
    return Times;
}

void BufferBlock::AddTimes()
{
    Times++;
}

void BufferBlock::ReadBlockFromDisk()
{
    ifstream fin;
    Dirty = false;
    Pin = false;
    Times = 0;
    fin.open(Filename, ios::in | ios::binary);
    if (!fin)
        return;
    fin.read(DataBlock.Data, BLOCKSIZE);
    fin.close();
}

void BufferBlock::WriteBlockToDisk()
{
    if (!Dirty)
        return;
    ofstream fout;
    fout.open(Filename, ios::out | ios::binary);
    if (!fout)
        return;
    fout.write(DataBlock.Data, BLOCKSIZE);
    fout.close();
}
