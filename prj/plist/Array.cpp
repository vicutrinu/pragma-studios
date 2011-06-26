/**
 *	@author	Victor Soria
 *	@file	Array.cpp
 **/

#include "Array.h"
#include "tinyxml/tinyxml.h"
#include "utils.h"
#include "Dictionary.h"

//----------------------------------------------------------------------------------------------------------------------
//	Array
//----------------------------------------------------------------------------------------------------------------------
Array::Array(const Array& aArray)
{
	for(int i = 0; i < aArray.Count(); ++i)
	{
		mContainer.push_back(new AnyData( Data(i) ) );
	}
}
	
//----------------------------------------------------------------------------------------------------------------------
//	Count
//----------------------------------------------------------------------------------------------------------------------
int Array::Count() const
{
	return (int)mContainer.size();
}

//----------------------------------------------------------------------------------------------------------------------
//	Data
//----------------------------------------------------------------------------------------------------------------------
const AnyData& Array::Data(int aPosition) const
{
	return *mContainer[aPosition];
}

//----------------------------------------------------------------------------------------------------------------------
//	Serialize
//----------------------------------------------------------------------------------------------------------------------
void Array::Serialize(TiXmlElement* array) const
{
	for(int i = 0; i < Count(); ++i)
	{
		switch(mContainer[i]->mType)
		{
			case AnyData::eString:
			{
				TiXmlElement* element = new TiXmlElement( "string" );
				TiXmlText* text = new TiXmlText( Data(i).mString );
				element->LinkEndChild(text);
				array->LinkEndChild(element);
				break;
			}
			case AnyData::eReal:
			{
				const char* lString = GetString( Data(i).mReal );
				TiXmlElement* element = new TiXmlElement( "real" );
				TiXmlText* text = new TiXmlText( lString );
				element->LinkEndChild(text);
				array->LinkEndChild(element);
				FreeString(lString);
				break;
			}
			case AnyData::eBool:
			{
				TiXmlElement* element = new TiXmlElement( Data(i).mBool? "true" : "false" );
				array->LinkEndChild(element);
				break;
			}
			case AnyData::eArray:
			{
				TiXmlElement* element = new TiXmlElement( "array" );
				array->LinkEndChild(element);
				Data(i).mArray->Serialize(element);
				break;
			}
			case AnyData::eDict:
			{
				TiXmlElement* element = new TiXmlElement( "dict" );
				array->LinkEndChild(element);
				Data(i).mDict->Serialize(element);
				break;
			}
			default: break;
		}
	}
}