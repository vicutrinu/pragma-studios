#pragma once

#include "plist/plist.h"
#include "plist/Array.h"
#include "plist/Dictionary.h"

class navigator
{
public:
	struct iterator
	{
		int mType; // 0 para llave/valor, 1 para valor de array

		void* mData;	// depende de mType es un KeyValue* o un AnyData*

		int next;
		int prev;
		int parent;
		int child;
	};
	
	navigator(const plist& aPlist)
	{
		enumerate(aPlist.GetRoot());		
	}
	
	const iterator* first() const
	{
		if(mContainer.size() > 0)
			return &mContainer[0];
		else
			return 0;
	}

	void log()
	{
		log(first(), 0);
	}

	void log(const iterator* aNode, int aTab) const
	{
		std::string lTab;
		int lCount = aTab;
		while(lCount--)
			lTab.push_back(' ');
		char lText[80];
		int i = 0;
		while(aNode)
		{
			if(aNode->mType == 0)
			{
				const KeyValue* lKeyValue = (const KeyValue*)aNode->mData;
				ToStr( lText, 80, lKeyValue->second );
				printf("%sKey <%s>, Value = %s\n", lTab.c_str(), lKeyValue->first.c_str(), lText);
			}
			else
			{
				const AnyData* lValue = (const AnyData*)aNode->mData;
				ToStr( lText, 80, *lValue );
				printf("%sIndex %d, Value = %s\n", lTab.c_str(), i, lText);
			}
			i++;
			if(aNode->child != -1)
				log( &mContainer[aNode->child], aTab+1 );
			if(aNode->next != -1)
				aNode = &mContainer[aNode->next];
			else
				aNode = 0;
		}
	}	
private:

	int enumerate(const Array& aArray)
	{
		int lRetVal = (int)mContainer.size();
		int lParent = (int)mContainer.size() - 1;
		int lPrev = -1;
		for(int i = 0; i < aArray.Count(); ++i)
		{
			mContainer.push_back( iterator() );
			iterator& lElement = mContainer.back();
			lElement.mType = 1;
			const AnyData* lEntry = &aArray.Data(i);
			lElement.mData = (void*)lEntry;
			lElement.parent = lParent;
			lElement.prev = (i == 0)? -1 : lPrev;
			lPrev = (int)mContainer.size()-1;
			
			if(lEntry->mType == AnyData::eDict)
			{
				int lChild = enumerate(*lEntry->mDict);
				mContainer[lPrev].child = lChild;
			}
			else if(lEntry->mType == AnyData::eArray)
			{
				int lChild = enumerate(*lEntry->mArray);
				mContainer[lPrev].child = lChild;
			}
			else
				lElement.child = -1;
			
            mContainer[lPrev].next = (i == aArray.Count()-1)? -1 : (int)mContainer.size();
		}
		mContainer.back().next =	 -1;
		
		return aArray.Count() > 0? lRetVal : -1;
	}
	
	int enumerate(const Dictionary& aDictionary)
	{
		int lRetVal = (int)mContainer.size();
		int lParent = (int)mContainer.size() - 1;
		int lPrev = -1;
		for(int i = 0; i < aDictionary.Count(); ++i)
		{
			mContainer.push_back( iterator() );
			iterator& lElement = mContainer.back();
			lElement.mType = 0;
			const KeyValue* lEntry = aDictionary.Entry(i);
			lElement.mData = (void*)lEntry;
			lElement.parent = lParent;
			lElement.prev = (i == 0)? -1 : lPrev;
			lPrev = (int)mContainer.size()-1;
			
			if(lEntry->second.mType == AnyData::eDict)
			{
				int lChild = enumerate(*lEntry->second.mDict);
				mContainer[lPrev].child = lChild;
			}
			else if(lEntry->second.mType == AnyData::eArray)
			{
				int lChild = enumerate(*lEntry->second.mArray);
				mContainer[lPrev].child = lChild;
			}
			else
				lElement.child = -1;
			
            mContainer[lPrev].next = (i == aDictionary.Count()-1)? -1 : (int)mContainer.size();
		}
		mContainer.back().next =	 -1;
		
		return aDictionary.Count() > 0? lRetVal : -1;
	}

	std::vector<iterator> mContainer;
};