/**
 *	@author	Victor Soria
 *	@file	Dictionary.cpp
 **/

#include "Dictionary.h"
#include "utils.h"
#include "Array.h"
#include "tinyxml/tinyxml.h"

//----------------------------------------------------------------------------------------------------------------------
//	Add
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
static inline T& Add(std::vector< KeyValue* >& aContainer, const Key& aKey, const T& aVal)
{
	AnyData lData;
	lData.Set<T>(aVal);
	aContainer.push_back( new KeyValue(aKey, lData) );
	return const_cast<T&>( aContainer.back()->second.Get<T>() );
}

//----------------------------------------------------------------------------------------------------------------------
//	Dictionary
//----------------------------------------------------------------------------------------------------------------------
Dictionary::Dictionary(const Dictionary& aDict)
{
	std::vector< KeyValue* >::const_iterator lIter = aDict.mContainer.begin();
	for ( ; lIter != aDict.mContainer.end(); ++lIter)
	{
		mContainer.push_back( new KeyValue((*lIter)->first, (*lIter)->second) );
	}
}

//----------------------------------------------------------------------------------------------------------------------
//	AddFloat
//----------------------------------------------------------------------------------------------------------------------
void Dictionary::AddFloat(const Key& aKey, float aFloat)
{
	Add<float>(mContainer, aKey, aFloat);
}

//----------------------------------------------------------------------------------------------------------------------
//	AddString
//----------------------------------------------------------------------------------------------------------------------
void Dictionary::AddString(const Key& aKey, const char* aString)
{
	Add<const char*>(mContainer, aKey, aString);
}

//----------------------------------------------------------------------------------------------------------------------
//	AddBool
//----------------------------------------------------------------------------------------------------------------------	
void Dictionary::AddBool(const Key& aKey, bool aBool)
{
	Add<bool>(mContainer, aKey, aBool);
}
	
//----------------------------------------------------------------------------------------------------------------------
//	AddArray
//----------------------------------------------------------------------------------------------------------------------
Array& Dictionary::AddArray(const Key& aKey)
{
	return Add<Array>(mContainer, aKey, Array());
}

//----------------------------------------------------------------------------------------------------------------------
//	AddDictionary
//----------------------------------------------------------------------------------------------------------------------
Dictionary& Dictionary::AddDictionary(const Key& aKey)
{
	return Add<Dictionary>(mContainer, aKey, Dictionary());
}

//----------------------------------------------------------------------------------------------------------------------
//	Count
//----------------------------------------------------------------------------------------------------------------------
int Dictionary::Count() const
{
	return (int)mContainer.size();
}

//----------------------------------------------------------------------------------------------------------------------
//	Name
//----------------------------------------------------------------------------------------------------------------------
const Key& Dictionary::Name(int aPosition) const
{
	return mContainer[aPosition]->first;
}

//----------------------------------------------------------------------------------------------------------------------
//	Data
//----------------------------------------------------------------------------------------------------------------------
const AnyData& Dictionary::Data(int aPosition) const
{
	return mContainer[aPosition]->second;
}

//----------------------------------------------------------------------------------------------------------------------
//	KeyValue
//----------------------------------------------------------------------------------------------------------------------
const KeyValue* Dictionary::Entry(int aPosition) const
{
	return mContainer[aPosition];
}

//----------------------------------------------------------------------------------------------------------------------
//	Serialize
//----------------------------------------------------------------------------------------------------------------------
void Dictionary::Serialize(TiXmlElement* dict) const
{
	for ( int i = 0; i < Count(); ++i)
	{
		TiXmlElement* key = new TiXmlElement( "key" );
		TiXmlText* text = new TiXmlText( Name(i).c_str() );
		key->LinkEndChild(text);
		dict->LinkEndChild(key);
		switch(Data(i).mType)
		{
			case AnyData::eString:
			{
				TiXmlElement* element = new TiXmlElement( "string" );
				TiXmlText* text = new TiXmlText( Data(i).mString );
				element->LinkEndChild(text);
				dict->LinkEndChild(element);
				break;
			}
			case AnyData::eReal:
			{
				TiXmlElement* element = new TiXmlElement( "real" );
				const char* lString = GetString( Data(i).mReal );
				TiXmlText* text = new TiXmlText( lString );
				element->LinkEndChild(text);
				FreeString(lString);
				dict->LinkEndChild(element);
				break;
			}
			case AnyData::eBool:
			{
				TiXmlElement* element = new TiXmlElement( Data(i).mBool? "true" : "false" );
				dict->LinkEndChild(element);
				break;
			}
			case AnyData::eArray:
			{
				TiXmlElement* element = new TiXmlElement( "array" );
				dict->LinkEndChild(element);
				Data(i).mArray->Serialize(element);
				break;
			}
			case AnyData::eDict:
			{
				TiXmlElement* element = new TiXmlElement( "dict" );
				dict->LinkEndChild(element);
				Data(i).mDict->Serialize(element);
				break;
			}
            default: break;
		}
	}
}