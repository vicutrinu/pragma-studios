/**
 *	@author	Victor Soria
 *	@file	utils.cpp
 **/

#include <stdio.h>

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