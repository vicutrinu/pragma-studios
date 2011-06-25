/*
 *  data.cpp
 *  pragma
 *
 *  Created by Victor on 26/12/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <pragma/types.h>
#include "data.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

namespace pragma
{

	//------------------------------------------------------------------------------------------------------------------
	// FloatToVoidPointer
	//------------------------------------------------------------------------------------------------------------------
	inline void* FloatToVoidPointer(float aFloat)
	{
		union TUnion
		{
			void* mVoid;
			float mFloat;
		};
		TUnion lData;
		lData.mFloat = aFloat;
		return lData.mVoid;
	}

	//------------------------------------------------------------------------------------------------------------------
	// VoidPointerToFloat
	//------------------------------------------------------------------------------------------------------------------
	inline float VoidPointerToFloat(void* aPointer)
	{
		union TUnion
		{
			void* mVoid;
			float mFloat;
		};
		TUnion lData;
		lData.mVoid = aPointer;
		return lData.mFloat;
	}

	//------------------------------------------------------------------------------------------------------------------
	// Value
	//------------------------------------------------------------------------------------------------------------------
	Value::Value(const Value& aValue)
	: mIsOk(false)
	, mType(eString)
	, mIsArray(false)
	, mCount(0)
	, mData(0) 
	{
		if(aValue.IsOk())
		{
			if(aValue.IsArray())
			{
				if(aValue.mType == eString)
				{
					Set("");
					SetAsArray(true);
					for(size_t i = 0; i < aValue.GetCount(); ++i)
					{
						const char* lString;
						aValue.GetAt(i, lString);
						SetAt(i, lString);
					}
				}
				else if(aValue.mType == eFloat)
				{
					Set(0.f);
					SetAsArray(true);
					for(size_t i = 0; i < aValue.GetCount(); ++i)
					{
						float lFloat;
						aValue.GetAt(i, lFloat);
						SetAt(i, lFloat);
					}
				}
			}
			else
			{
				if(aValue.mType == eString)
				{
					const char* lString;
					aValue.Get(lString);
					Set(lString);
				}
				else if(aValue.mType == eFloat)
				{
					float lFloat;
					aValue.Get(lFloat);
					Set(lFloat);
				}
			}

		}
	}
	//------------------------------------------------------------------------------------------------------------------
	// Clear
	//------------------------------------------------------------------------------------------------------------------
	void Value::Clear()
	{
		if(IsOk())
		{
			if(!IsArray())
				ClearAt(&mData);
			else
			{
				void** lArray = (void**)mData;
				for (size_t i = 0; i < mCount; ++i)
					ClearAt(&lArray[i]);
				free(mData);
			}
		}
		
		mData = 0;
		mType = eString;
		mCount = 0;
		mIsArray = false;
		mIsOk = false;
	}

	//------------------------------------------------------------------------------------------------------------------
	// SetAsArray
	//------------------------------------------------------------------------------------------------------------------
	void Value::SetAsArray(bool aIsArray)
	{
		if(!IsOk())
			return;
		
		if(aIsArray)
		{
			if(!IsArray())
			{
				void** lNew = (void**)malloc(sizeof(void*));
				memcpy(lNew, &mData, sizeof(void*));
				mData = (void*)lNew;
				mCount = 1;
			}
		}
		else
		{
			if(IsArray() && mCount > 0)
			{
				switch (mType)
				{
					case eString:
					{
						const char* lString;
						GetAt(0, lString);
						Value lVal;
						lVal.Set(lString);
						lVal.Get(lString);
						Clear();
						mIsArray = false;
						Set(lString);
						break;
					}
					case eFloat:
					{
						float lFloat;
						GetAt(0, lFloat);
						Clear();
						mIsArray = false;
						Set(lFloat);
						break;
					}
					default:
						break;
				}
			}
		}

		mIsArray = aIsArray;
	}

	//------------------------------------------------------------------------------------------------------------------
	// ClearAt
	//------------------------------------------------------------------------------------------------------------------
	void Value::ClearAt(void** aPtr)
	{
		switch (mType)
		{
			case eString:
			{
				char* lString = (char*)*aPtr;
				if(lString)
				{
					free((void*)lString);
				}
				break;
			}
			case eFloat:
				break;
			default:
				break;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	// Set
	//------------------------------------------------------------------------------------------------------------------
	void Value::Set(const char* aValue)
	{
		if(IsArray())
			return;
		
		if(IsOk())
			Clear();
		mType = eString;
		if(aValue)
		{
			SetAt(&mData, aValue);
			mIsOk = true;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	// Get
	//------------------------------------------------------------------------------------------------------------------
	bool Value::Get(const char*& aString) const
	{
		if(IsOk() && !IsArray() && mType == eString)
		{
			return GetAt((void**)&mData, aString);
		}
		else
		{
			aString = 0;
			return false;
		}

	}

	//------------------------------------------------------------------------------------------------------------------
	// Set
	//------------------------------------------------------------------------------------------------------------------
	void Value::Set(float aValue)
	{
		if(IsOk())
		{
			Clear();
		}
		mType = eFloat;
		mIsArray = false;
		SetAt(&mData, aValue);
		mIsOk = true;
	}

	//------------------------------------------------------------------------------------------------------------------
	// Get
	//------------------------------------------------------------------------------------------------------------------
	bool Value::Get(float& aFloat) const
	{
		if(IsOk() && !IsArray() && mType == eFloat)
		{
			return GetAt((void**)&mData, aFloat);
		}
		else 
		{
			aFloat = 0;
			return false;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	// SetAt
	//------------------------------------------------------------------------------------------------------------------
	void Value::SetAt(int aPosition, const char* aValue)
	{
		if(!IsOk())
			return;
		
		if(!IsArray())
			return;
		
		if(mType != eString)
			return;
		
		if(aPosition == mCount)
		{	// resize pointers table
			void* lNew = (void*)malloc(sizeof(void*) * (mCount + 1));
			memcpy(lNew, mData, sizeof(void*) * mCount);
			free(mData);
			mData = lNew;
			void** lArray = (void**)mData;
			SetAt(&lArray[mCount], aValue);
			mCount++;
		}
		else if(mCount > 0 && aPosition >= 0 && aPosition < mCount)
		{
			void** lArray = (void**)mData;
			SetAt(&lArray[aPosition], aValue);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	// GetAt
	//------------------------------------------------------------------------------------------------------------------
	bool Value::GetAt(int aPosition, const char*& aValue) const
	{
		if(!IsArray())
			return false;
		
		if(mCount > 0 && aPosition >= 0 && aPosition < mCount)
		{
			void** lArray = (void**)mData;
			return GetAt(&lArray[aPosition], aValue);
		}
		else 
			return false;
	}

	//------------------------------------------------------------------------------------------------------------------
	// SetAt
	//------------------------------------------------------------------------------------------------------------------
	void Value::SetAt(int aPosition, float aValue)
	{
		if(!IsOk())
			return;
		
		if(!IsArray())
			return;
		
		if(mType != eFloat)
			return;
		
		if(aPosition == mCount)
		{	// resize pointers table
			void* lNew = (void*)malloc(sizeof(void*) * (mCount + 1));
			memcpy(lNew, mData, sizeof(void*) * mCount);
			free(mData);
			mData = lNew;
			void** lArray = (void**)mData;
			SetAt(&lArray[mCount], aValue);
			mCount++;
		}
		else if(mCount > 0 && aPosition >= 0 && aPosition < mCount)
		{
			void** lArray = (void**)mData;
			SetAt(&lArray[aPosition], aValue);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	// GetAt
	//------------------------------------------------------------------------------------------------------------------
	bool Value::GetAt(int aPosition, float& aValue) const
	{
		if(!IsArray())
			return false;
		
		if(mCount > 0 && aPosition >= 0 && aPosition < mCount)
		{
			void** lArray = (void**)mData;
			return GetAt(&lArray[aPosition], aValue);
		}
		else 
			return false;
	}

	//------------------------------------------------------------------------------------------------------------------
	// SetAt
	//------------------------------------------------------------------------------------------------------------------
	void Value::SetAt(void** aPtr, const char* aValue)
	{
		char* lString;
		lString = (char*)malloc(strlen(aValue) + 1);
		strcpy(lString, aValue);
		*aPtr = (void*)lString;
	}

	//------------------------------------------------------------------------------------------------------------------
	// GetAt
	//------------------------------------------------------------------------------------------------------------------
	bool Value::GetAt(void** aPtr, const char*& aString) const
	{
		aString = (char*)*aPtr;
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	// SetAt
	//------------------------------------------------------------------------------------------------------------------
	void Value::SetAt(void** aPtr, float aValue)
	{
		*aPtr = FloatToVoidPointer(aValue);
	}

	//------------------------------------------------------------------------------------------------------------------
	// GetAt
	//------------------------------------------------------------------------------------------------------------------
	bool Value::GetAt(void** aPtr, float& aFloat) const
	{
		aFloat = VoidPointerToFloat(*aPtr);
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	// PropertyList_UnitTest
	//------------------------------------------------------------------------------------------------------------------
	bool PropertyList_UnitTest()
	{
		Value lVal;
		lVal.Set("mierda");
		lVal.SetAsArray(true);
		lVal.SetAt(1, "es");
		lVal.SetAt(2, "caca");
		lVal.SetAt(2, 1234);	// ignore
		
		for(size_t i = 0; i < 3; ++i)
		{
			const char* lStr;
			lVal.GetAt(i, lStr);
			printf("%s\n", lStr);
		}

		PropertyList lList;
		lList.Set(string("HolaArray"), lVal);
		
		lVal.SetAsArray(false);
		const char* lStr;
		lVal.Get(lStr);
		printf("%s\n", lStr);
		lList.Set(string("HolaStr"), lVal);

		lVal.Clear();
		lVal.Set(1234);
		float lFloat;
		lVal.Get(lFloat);
		printf("%f\n", lFloat);
		lList.Set(string("HolaFloat"), lVal);

		lList.Log();
		
		printf("%f\n", lList.GetFloat(string("HolaFloat"), -1));
		printf("%s\n", lList.GetString(string("HolaStr"), string("caca")).c_str());
		return true;
	}
	
	void PropertyList::Set(const string& aKey, const Value& aValue)
	{
		std::pair<string, Value> lPair(aKey, aValue);
		mList.push_back(lPair);
	}
	
	Value PropertyList::Get(const string& aKey, const Value& aDefault)
	{
		for(size_t i = 0; i < mList.size(); ++i)
		{
			if(mList[i].first == aKey)
			{
				return mList[i].second;
			}
		}
		return aDefault;
	}
	
	void PropertyList::SetString(const string& aKey, const string& aString)
	{
		Value lDefault;
		lDefault.Set(aString.c_str());
		Set(aKey, lDefault);
	}
	
	void PropertyList::SetFloat(const string& aKey, const float aFloat)
	{
		Value lDefault;
		lDefault.Set(aFloat);
		Set(aKey, lDefault);		
	}
	
	string PropertyList::GetString(const string& aKey, const string& aDefault /*= string::Empty*/)
	{
		Value lDefault;
		lDefault.Set(aDefault.c_str());
		Value lRetVal = Get(aKey, lDefault);
		const char* lStr;
		lRetVal.Get(lStr);
		return(string(lStr));
	}
	
	float PropertyList::GetFloat(const string& aKey, float aDefault /*= 0*/)
	{
		Value lDefault;
		lDefault.Set(aDefault);
		Value lRetVal = Get(aKey, lDefault);
		float lFloat;
		lRetVal.Get(lFloat);
		return lFloat;
	}
	
	
	void PropertyList::Log()
	{
		for(size_t i = 0; i < mList.size(); ++i)
		{
			printf("Key: %s\n", mList[i].first.c_str());
			printf("Value:");
			if(mList[i].second.IsArray())
			{
				switch( mList[i].second.GetType() )
				{
					case Value::eString: 
					{
						for(size_t j = 0; j < mList[i].second.GetCount(); ++j)
						{
							const char* lString;
							mList[i].second.GetAt(j, lString);
							printf("%s", lString);
							if(j < mList[i].second.GetCount() - 1)
								printf(", ");
							else 
								printf("\n");
						}
						break;
					}
					case Value::eFloat:
					{
						for(size_t j = 0; j < mList[i].second.GetCount(); ++j)
						{
							float lFloat;
							mList[i].second.GetAt(j, lFloat);
							printf("%f", lFloat);
							if(j < mList[i].second.GetCount() - 1)
								printf(", ");
							else 
								printf("\n");
						}
						break;
					}
					default:
						break;
				}
			}
			else
			{
				switch( mList[i].second.GetType() )
				{
					case Value::eString: 
					{
						const char* lString;
						mList[i].second.Get(lString);
						printf("%s\n", lString);
						break;
					}
					case Value::eFloat:
					{
						float lFloat;
						mList[i].second.Get(lFloat);
						printf("%f\n", lFloat);
						break;
					}
					default:
						break;
				}
			}

		}
	}

}
