/*
 *  string.cpp
 *  pragma
 *
 *  Created by Victor on 30/12/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <pragma/data/strings.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

namespace pragma
{
	string string::Empty(0);
	
	string::string(const char* aString)
	: mEntry(0)
	{
		if(aString == 0)
			return;
		
		static Entry* sFirst = 0;
		bool lFound = false;
		if(sFirst != 0)
		{
			Entry* lEntry = sFirst;
			while(lEntry != 0)
			{
				if(strcmp(lEntry->mText, aString) == 0)
				{
					mEntry = lEntry;
					mEntry->mRefCount++;
					lFound = true;
					break;
				}
				else
				{
					lEntry = lEntry->mNext;
				}
			}
		}
		
		if(!lFound)
		{
			Entry* lEntry = (Entry*)malloc(sizeof(Entry));
			lEntry->mText = (const char*)malloc(strlen(aString) + 1);
			strcpy(const_cast<char*>(lEntry->mText), aString);
			lEntry->mNext = sFirst;
			lEntry->mRefCount = 1;
			sFirst = lEntry;
			mEntry = lEntry;
		}
	}
	
	string::string(const string& aString)
	{
		mEntry = aString.mEntry;
		if(mEntry)
			mEntry->mRefCount++;
	}
	
	string& string::operator=(const string& aString)
	{
		mEntry = aString.mEntry;
		if(mEntry)
			mEntry->mRefCount++;
		return *this;
	}
	
	string::string()
	{
		if(mEntry)
		{
			mEntry->mRefCount--;
		}
	}
	
	bool string::operator==(const string& aString)
	{
		return mEntry == aString.mEntry;
	}
}
