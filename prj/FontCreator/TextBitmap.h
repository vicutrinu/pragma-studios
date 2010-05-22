#pragma once

#include <pragma/pragma.h>

namespace pragma
{
	class Font;
	
	bool CreateTextBitmap(const Font& aFont, const char* aText, uint8* aBitmap, vector2f& aSize, vector2f& aStart);
}
