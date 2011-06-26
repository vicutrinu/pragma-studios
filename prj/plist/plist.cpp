/**
 *	@author	Victor Soria
 *	@file	plist.cpp
 **/

#include "plist.h"

#include "tinyxml/tinyxml.h"
#include "Dictionary.h"

void plist::Serialize(const char* aFilename)
{
	TiXmlDocument doc;
	TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "UTF-8", "" );
	doc.LinkEndChild( decl );
	TiXmlElement* plist = new TiXmlElement( "plist" );
	plist->SetAttribute("version", "1.0");
	doc.LinkEndChild( plist );
	TiXmlElement* dict = new TiXmlElement( "dict" );
	plist->LinkEndChild(dict);
	mRoot.Serialize(dict);
	doc.SaveFile( aFilename );
}
