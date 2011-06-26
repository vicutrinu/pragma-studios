/**
 *	@author	Victor Soria
 *	@file	AnyData.cpp
 **/

#include "AnyData.h"
#include "Array.h"
#include "Dictionary.h"

//- Template Instantiation -//
template<> void                 AnyData::Set<float>         (const float& aVal);
template<> const float&         AnyData::Get<float>         () const;
template<> void                 AnyData::Set<AnyData::str>  (const str& aVal);
template<> const                AnyData::str& AnyData::Get<AnyData::str>() const;
template<> void                 AnyData::Set<bool>          (const bool& aVal);
template<> const bool&          AnyData::Get<bool>          () const;
template<> void                 AnyData::Set<Array>         (const Array& aVal);
template<> const Array&         AnyData::Get<Array>         () const;
template<> void                 AnyData::Set<Dictionary>    (const Dictionary& aVal);
template<> const Dictionary&    AnyData::Get<Dictionary>    () const;


//-------------------------------------------------------------------------
//  Constructor
//-------------------------------------------------------------------------
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


//-------------------------------------------------------------------------
//  Destructor
//-------------------------------------------------------------------------
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


//-------------------------------------------------------------------------
//  Set
//-------------------------------------------------------------------------
template<>
void AnyData::Set<float>(const float& aVal)
{
	mType = eReal;
	mReal = aVal;
}


//-------------------------------------------------------------------------
//  Get
//-------------------------------------------------------------------------
template<>
const float& AnyData::Get<float>() const
{
	return mReal;
}


//-------------------------------------------------------------------------
//  Set
//-------------------------------------------------------------------------
template<>
void AnyData::Set<AnyData::str>(const str& aVal)
{
	mType = eString;
	size_t lLen = strlen(aVal);
	mString = (char*)malloc(lLen+1);
	strcpy(mString, aVal);
}


//-------------------------------------------------------------------------
//  Get
//-------------------------------------------------------------------------
template<>
const AnyData::str& AnyData::Get<AnyData::str>() const
{
	return (str&)mString;
}


//-------------------------------------------------------------------------
//  Set
//-------------------------------------------------------------------------
template<>
void AnyData::Set<bool>(const bool& aVal)
{
	mType = eBool;
	mBool = aVal;
}


//-------------------------------------------------------------------------
//  Get
//-------------------------------------------------------------------------
template<>
const bool& AnyData::Get<bool>() const
{
	return mBool;
}


//-------------------------------------------------------------------------
//  Set
//-------------------------------------------------------------------------
template<>
void AnyData::Set<Array>(const Array& aVal)
{
	mType = eArray;
	mArray = new Array(aVal);
}


//-------------------------------------------------------------------------
//  Get
//-------------------------------------------------------------------------
template<>
const Array& AnyData::Get<Array>() const
{
	return *mArray;
}


//-------------------------------------------------------------------------
//  Set
//-------------------------------------------------------------------------
template<>
void AnyData::Set<Dictionary>(const Dictionary& aVal)
{
	mType = eDict;
	mDict = new Dictionary(aVal);
}

//-------------------------------------------------------------------------
//  Get
//-------------------------------------------------------------------------
template<>
const Dictionary& AnyData::Get<Dictionary>() const
{
	return *mDict;
}

