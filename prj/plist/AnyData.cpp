/**
 *	@author	Victor Soria
 *	@file	AnyData.cpp
 **/

#include "AnyData.h"
#include "Array.h"
#include "Dictionary.h"

typedef const char* str;

template<> void AnyData::Set<float>(const float& aVal);
template<> const float& AnyData::Get<float>() const;
template<> void AnyData::Set<str>(const str& aVal);
template<> const str& AnyData::Get<str>() const;
template<> void AnyData::Set<bool>(const bool& aVal);
template<> const bool& AnyData::Get<bool>() const;
template<> void AnyData::Set<Array>(const Array& aVal);
template<> const Array& AnyData::Get<Array>() const;
template<> void AnyData::Set<Dictionary>(const Dictionary& aVal);
template<> const Dictionary& AnyData::Get<Dictionary>() const;

AnyData::AnyData(const AnyData& aData)
{
	mType = aData.mType;
	switch(aData.mType)
	{
		case eString: this->Set<str>( aData.Get<str>() ); break;
		case eReal: this->Set<float>( aData.Get<float>() ); break;
		case eBool: this->Set<bool>( aData.Get<bool>() ); break;
		case eArray: this->Set<Array>( aData.Get<Array>() ); break;
		case eDict: this->Set<Dictionary>( aData.Get<Dictionary>() ); break;
	}
}

AnyData::~AnyData()
{
	switch(mType)
	{
		case eArray: delete mArray; break;
		case eDict:  delete mDict;	break;
		case eString:free(mString); mString = 0; break;
		default: 					break;
	}
}

template<>
void AnyData::Set<float>(const float& aVal)
{
	mType = eReal;
	mReal = aVal;
}

template<>
const float& AnyData::Get<float>() const
{
	return mReal;
}

template<>
void AnyData::Set<str>(const str& aVal)
{
	mType = eString;
	size_t lLen = strlen(aVal);
	mString = (char*)malloc(lLen+1);
	strcpy(mString, aVal);
}

template<>
const str& AnyData::Get<str>() const
{
	return (str&)mString;
}

template<>
void AnyData::Set<bool>(const bool& aVal)
{
	mType = eBool;
	mBool = aVal;
}

template<>
const bool& AnyData::Get<bool>() const
{
	return mBool;
}

template<>
void AnyData::Set<Array>(const Array& aVal)
{
	mType = eArray;
	mArray = new Array(aVal);
}

template<>
const Array& AnyData::Get<Array>() const
{
	return *mArray;
}

template<>
void AnyData::Set<Dictionary>(const Dictionary& aVal)
{
	mType = eDict;
	mDict = new Dictionary(aVal);
}

template<>
const Dictionary& AnyData::Get<Dictionary>() const
{
	return *mDict;
}

//----------------------------------------------------------------------------------------------------------------------
//	ToStr
//----------------------------------------------------------------------------------------------------------------------
void ToStr(char* aDest, int aLength, const AnyData& aData)
{
	switch(aData.GetType())
	{
	case AnyData::eString:
		snprintf(aDest, aLength, "%s", aData.Get<str>());
		break;
	case AnyData::eReal:
		snprintf(aDest, aLength, "%f", aData.Get<float>());
		break;
	case AnyData::eBool:
		snprintf(aDest, aLength, "%s", aData.Get<bool>()? "true" : "false" );
		break;
	case AnyData::eArray:
		snprintf(aDest, aLength, "<array>");
		break;
	case AnyData::eDict:
		snprintf(aDest, aLength, "<dict>");
		break;
	}
}