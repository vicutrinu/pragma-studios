/*
 *  data.h
 *  pragma
 *
 *  Created by Victor on 26/12/10.
 *  Copyright 2010 #Pragma Studios. All rights reserved.
 *
 */

#include <pragma/types.h>
#include <vector>

namespace pragma
{
	
	bool PropertyList_UnitTest();
	
	class Value
	{
	public:
		enum Type
		{
			eString	= 0,
			eFloat	= 1,
		};

					Value		() : mIsOk(false), mType(eString), mIsArray(false), mCount(0), mData(0) { }
					Value		(const Value& aValue);
					~Value		() { Clear(); }
		
		bool		IsOk		() const { return mIsOk; }
		void		Clear		();
		
		void		SetAsArray (bool aIsArray);
		
		void		Set			(const char* aValue);		
		bool		Get			(const char*& aString) const;

		void		Set			(float aValue);
		bool		Get			(float& aFloat) const;
		
		void		SetAt		(int aPosition, const char* aValue);
		bool		GetAt		(int aPosition, const char*& aValue) const;
		void		SetAt		(int aPosition, float aValue);
		bool		GetAt		(int aPosition, float& aValue) const;
		
		bool		IsArray		() const { return mIsArray; }
		unsigned	GetCount	() const { return mCount; }
		Type		GetType		() const { return mType; }
		
	private:
		
		void		ClearAt		(void** aPtr);

		void		SetAt		(void** aPtr, const char* aValue);
		bool		GetAt		(void** aPtr, const char*& aString) const;
		void		SetAt		(void** aPtr, float aValue);
		bool		GetAt		(void** aPtr, float& aFloat) const;

		bool		mIsOk;
		Type		mType;
		unsigned	mCount;
		bool		mIsArray;
		
		void*		mData;
	};
	
	class PropertyList
	{
	public:
		void Set(const string& aKey, const Value& aValue);
		Value Get(const string& aKey, const Value& aDefault);
		
		void SetString(const string& aKey, const string& aString);
		void SetFloat(const string& aKey, const float aFloat);
		
		string GetString(const string& aKey, const string& aDefault = string());
		float  GetFloat(const string& aKey, float aDefault = 0);
		
		void Log();
	private:
		std::vector< std::pair<string, Value> > mList;
	};
	
}
