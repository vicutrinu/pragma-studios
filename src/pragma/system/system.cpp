#include "system.h"

namespace pragma
{
	std::string GetPath(const char* aCompletePath)
	{
		std::string lRetVal = aCompletePath;
		while( lRetVal.length() &&
			   ( lRetVal[lRetVal.length()-1] != '\\' &&
			     lRetVal[lRetVal.length()-1] != '/' ) )
		{
			lRetVal.resize(lRetVal.length()-1);
		}

		return lRetVal;
	}
}
