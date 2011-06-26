/**
 *	@author	Victor Soria
 *	@file	Array.h
 **/

#pragma once

#include <vector>
#include "AnyData.h"

class TiXmlElement;

class Array
{
public:
					Array				() { }
					Array				(const Array& aArray);
	
	void 			InsertFloat			(int aPosition, float aValue) { Insert<float>(aPosition, aValue); }
	void 			InsertBool			(int aPosition, bool aValue) { Insert<bool>(aPosition, aValue); }
	void 			InsertString		(int aPosition, const char* aString) { Insert<const char*>(aPosition, aString); }
	void 			InsertArray			(int aPosition, const Array& aArray) { Insert<Array>(aPosition, aArray); }
	void 			InsertDictionary	(int aPosition, const Dictionary& aDictionary) { Insert<Dictionary>(aPosition, aDictionary); }

	int 			Count				() const;
	const AnyData& 	Data				(int aPosition) const;

	void 			Serialize			(TiXmlElement* array) const;
private:
	template<typename T>
	void Insert(int aPosition, const T& aVal)
	{
		mContainer.insert(mContainer.begin()+aPosition, new AnyData());
        mContainer[aPosition]->Set<T>(aVal);
	}

	std::vector<AnyData*> mContainer;
};
