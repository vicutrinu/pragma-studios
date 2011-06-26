/**
 *	@author	Victor Soria
 *	@file	utils.cpp
 **/

#include <stdio.h>
#include <plist/AnyData.h>

#if _MSC_VER
#define snprintf _snprintf
#endif

const char* GetString(float aReal)
{
	int lLen = snprintf(0,0,"%f", aReal);
	char* lRetVal = new char[lLen+1];
	snprintf(lRetVal, lLen+1, "%f", aReal);
	return lRetVal;
}

void FreeString(const char* aString)
{
	delete[] aString;
}

//----------------------------------------------------------------------------------------------------------------------
//	ToStr
//----------------------------------------------------------------------------------------------------------------------
void ToStr(char* aDest, int aLength, const AnyData& aData)
{
    switch(aData.GetType())
    {
    case AnyData::eString:
        snprintf(aDest, aLength, "%s", aData.Get<AnyData::str>());
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