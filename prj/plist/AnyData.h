/**
 *	@author	Victor Soria
 *	@file	AnyData.h
 **/

#pragma once

class Array;
class Dictionary;

struct AnyData
{
    typedef const char* str;

	AnyData		() { }
	AnyData		(const AnyData& aData);
	~AnyData	();
	
	enum EType
	{
		eString,
		eReal,
		eBool,
		eArray,
		eDict
	};
	EType mType;
	
	union
	{
		float 			mReal;
		char*			mString;
		bool 			mBool;
		Array* 			mArray;
		Dictionary*		mDict;
	};
	
	template<typename T>
	void Set(const T& aVal);
	
	template<typename T>
	const T& Get() const;
	
	EType GetType() const { return mType; }
};

void ToStr(char* aDest, int aLength, const AnyData& aData);