/*
 *  string.h
 *  pragma
 *
 *  Created by Victor on 30/12/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once 
namespace pragma
{
	
	class string
	{
	private:
		struct Entry
		{
			const char* mText;
			Entry*		mNext;
			int			mRefCount;
		};
	public:
		string(const char* aString);
		string(const string& aString);
		string();
		
		string& operator=(const string& aString);
		operator const char*() const { return GetStr(); }
		bool operator==(const string& aString);
		
		const char* GetStr() const { return mEntry == 0? 0 : mEntry->mText; }
		
		static string Empty;
	private:
		Entry* mEntry;
	};
	
}

