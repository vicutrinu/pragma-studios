#include "TextBitmap.h"
#include "Font.h"

namespace pragma
{
	/**
	 *	CreateTextBitmap
	 */
	bool CreateTextBitmap(const Font& aFont, const char* aText, uint8* aBitmap, vector2f& aSize, vector2f& aStart)
	{
		vector2f lSize(0,0);
		
		if(aText == 0)
		{
			aSize = lSize;
			return false;
		}

		// Calculate required bitmap size depending on the text
		int lMaxY = -1;
		int lMinY = 1;
		const char* lText = aText;
		while(*lText)
		{
			const Font::Glyph* lGlyph = aFont.GetGlyph(*lText++);
			if(lGlyph)
			{
				if(lMaxY < lMinY)
				{
					lMaxY = lGlyph->mStartY;
					lMinY = -lGlyph->mHeight + lGlyph->mStartY;
					aStart.x = lGlyph->mStartX;
				}
				else
				{
					lMaxY = max<int>(lMaxY, lGlyph->mStartY);
					lMinY = min<int>(lMinY, -lGlyph->mHeight + lGlyph->mStartY);
				}
				lSize.x+= (float)lGlyph->mAdvanceX / 64;
				lSize.y = lMaxY - lMinY;
			}
		}

		aStart.y = lMaxY;
		if(aStart.y < 0)
		{
			return false;
		}

		// If the bitmap passed is big enough, draw the text inside it
		if(aBitmap && aSize.x >= lSize.x && aSize.y >= aSize.y)
		{
			memset(aBitmap, 0, int(aSize.x * aSize.y));
			uint8* lBasePtr = aBitmap;
			lBasePtr+= (int)aStart.y * (int)aSize.x - (int)aStart.x;	// Pointer to the Base Line
			lText = aText;
			while(*lText)
			{
				const Font::Glyph* lGlyph = aFont.GetGlyph(*lText++);
				if(lGlyph)
				{
					uint8* lPtr = lBasePtr - (lGlyph->mStartY * (int)aSize.x) + lGlyph->mStartX;
					const uint8* lSrc = lGlyph->mBitmap;
					for(size_t j = 0; j < lGlyph->mHeight; ++j)
					{
						for(size_t i = 0; i < lGlyph->mWidth; ++i)
							*lPtr++= *lSrc++;
						lPtr+= (int)aSize.x - lGlyph->mWidth;
					}
					lBasePtr+= lGlyph->mAdvanceX >> 6;
				}
			}			
		}
		aSize = lSize;
		return true;
	}
}
