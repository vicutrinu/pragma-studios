/**
 *	@author	Victor Soria
 *	@file	plist.h
 **/

#pragma once

#include "Dictionary.h"

class plist
{
public:
	Dictionary& GetRoot() { return mRoot; }
	const Dictionary& GetRoot() const { return mRoot; }
	
	void Serialize(const char* aFilename);
	
private:
	Dictionary mRoot;
};