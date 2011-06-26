/**
 *	@author	Victor Soria
 *	@file	Dictionary.h
 **/

#pragma once

#include <vector>
#include <string>
#include "AnyData.h"

typedef std::string Key;

struct KeyValue
{
	KeyValue(const Key& aKey, const AnyData& aAnyData) : first(aKey), second(aAnyData) { }

	Key 	first;
	AnyData second;
};

class TiXmlElement;

class Dictionary
{
public:
					Dictionary		() { }
					Dictionary		(const Dictionary& aDict);
	
	void 			AddFloat		(const Key& aKey, float aFloat);
	void 			AddString		(const Key& aKey, const char* aString);
	void 			AddBool			(const Key& aKey, bool aBool);
	Array& 			AddArray		(const Key& aKey);
	Dictionary& 	AddDictionary	(const Key& aKey);
	
	int 			Count			() const;
	const Key& 		Name			(int aPosition) const;
	const AnyData&	Data			(int aPosition) const;
	const KeyValue* Entry			(int aPosition) const;

	void 			Serialize		(TiXmlElement* dict) const;
private:

	std::vector< KeyValue* > mContainer;
};