/**
 *	@author	Victor Soria
 *	@file	main.cpp
 **/

#include "plist.h"
#include "Array.h"
#include "Dictionary.h"
#include "navigator.h"

int main(int argc, char* argv[])
{		
	plist lProperties;
	lProperties.GetRoot().AddString("mierder", "mierder" );
	Dictionary& lDict = lProperties.GetRoot().AddDictionary("diccionario");
	lDict.AddFloat("zasca!", -1);
	Array& lArray = lDict.AddArray("array");
	lArray.InsertFloat(0, 0);
	lArray.InsertString(0, "me molo mazo");
	lArray.InsertDictionary(2, Dictionary());
	lProperties.GetRoot().AddFloat("numeraco", 1234);
	lProperties.GetRoot().AddBool("booleano", false);
	lProperties.GetRoot().AddString("a", "a");
	lProperties.GetRoot().AddString("c", "x");
	lProperties.GetRoot().AddString("b", "l");
	lProperties.Serialize("madeByHand.plist");
	
	navigator lNav(lProperties);
	lNav.log();

	return 0;
}