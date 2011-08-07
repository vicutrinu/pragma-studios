//
//  Key.cpp
//  pragma
//
//  Created by Victor on 05/02/11.
//  Copyright 2011 #Pragma Studios. All rights reserved.
//

#include "Key.h"

#include <string.h>
#include <stdlib.h>

struct StringEntry
{
    const char*     mString;
    StringEntry*    mNext;
    unsigned        mRefCount;
};

StringEntry*    sTable[256];

StringEntry     sPool[1024];
unsigned        sNextFree = 0;

StringEntry*    sFreePointers[1024];
int             sNextFreePointer = -1;

static inline StringEntry* FindEntry(StringEntry* aFirst, const char* aString)
{
    if(aFirst == 0)
        return 0;
    
    while(strcmp(aFirst->mString, aString) != 0)
    {
        aFirst = aFirst->mNext;
        if(aFirst == 0)
            return 0;
    }
    return aFirst;
}

Key::Key(const char* aKey)
{
    if(aKey)
    {
        StringEntry* lEntry = sTable[(unsigned)*aKey];
        lEntry = FindEntry(lEntry, aKey);
        if(lEntry)
        {   // Found
            lEntry->mRefCount++;
            mID = lEntry - &sPool[0];
        }
        else
        {   // Not found, allocate new one
            size_t lLen = strlen(aKey);
            char* lPtr = (char*)malloc(lLen+1);
            strcpy(lPtr, aKey);
            
            StringEntry* lNewEntry;
            if(sNextFreePointer >= 0)
            {
                lNewEntry = sFreePointers[sNextFreePointer];
                sNextFreePointer--;
            }
            else
                lNewEntry = &sPool[sNextFree++];
            
            StringEntry* lPrevEntry = sTable[(unsigned)*aKey];
            lNewEntry->mString = lPtr;
            lNewEntry->mNext = 0;
            lNewEntry->mRefCount = 1;
            lEntry = sTable[(unsigned)*aKey];
            if(lEntry == 0)
                sTable[(unsigned)*aKey] = lNewEntry;
            else
            {
                while(lPrevEntry->mNext)
                    lPrevEntry = lPrevEntry->mNext;
                lPrevEntry->mNext = lNewEntry;
            }
            mID = lNewEntry - &sPool[0];
        }
    }
    else
        mID = -1;
}

Key::Key(const Key& aKey)
{
    mID = aKey.mID;
    sPool[mID].mRefCount++;
}

Key::~Key()
{
    StringEntry* lEntry = &sPool[mID];
    lEntry->mRefCount--;
    if(lEntry->mRefCount == 0)
    {
        unsigned lChar = (unsigned)lEntry->mString[0];
        StringEntry* lPtr = sTable[lChar];
        if(lPtr == lEntry)
        {
            sTable[lChar] = lPtr->mNext;
        }
        else
        {
            while(lPtr->mNext != lEntry)
                lPtr = lPtr->mNext;
            lPtr->mNext = lEntry->mNext;
        }
        free((void*)lEntry->mString);
        sNextFreePointer++;
        sFreePointers[sNextFreePointer] = lEntry;
    }
}

const char* Key::c_str()
{
    if(mID >= 0)
        return sPool[mID].mString;
    else
        return 0;
}
