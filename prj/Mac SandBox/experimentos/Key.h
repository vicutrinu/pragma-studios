//
//  Key.h
//  pragma
//
//  Created by Victor on 05/02/11.
//  Copyright 2011 #Pragma Studios. All rights reserved.
//

#pragma once

class Key
{
public:
                Key     ( const char* aKey );
                Key     ( const Key& aKey );
                ~Key    ();
    
    const char* c_str   ();
    bool        IsOk    () { return mID >= 0; }
private:
    int mID;
};

class PropertyTable
{
    void        AddValue    (Key aKey, void* aData, unsigned aBytes, unsigned aTypeID);
    unsigned    GetValue    (Key aKey, void** aData, unsigned aDataBufferLength, unsigned aTypeID);
};