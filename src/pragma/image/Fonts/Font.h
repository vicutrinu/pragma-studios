#pragma once

#include <pragma/types.h>
#include <map>

namespace pragma
{
	/**
	 *	@class Font
	 */
	class Font
	{
	public:
		struct Glyph
		{
			uint32	mCode;			// Character code
			uint8*  mBitmap;		// Buffer containing a grayscale antialiased character bitmap
			size_t	mWidth;			// Width of the bitmap
			size_t	mHeight;		// Height of the bitmap
			int		mStartX;		// Horizontal offset of the bitmap
			int		mStartY;		// Vertical offset of the bitmap
			int		mAdvanceX;		// Horizontal increment after drawing the glyph. In 26.6 fixed point format
		};
		
		typedef void (*EnumerationCallback)(const Glyph& aGlyph);
		
	public:
						Font		();
		
		void			Init		(size_t aSize);
		void			End			();
		bool			IsOk		() const { return mOk; }

		void			AddGlyph	(Glyph& aGlyph);
		
		const Glyph*	GetGlyph	(uint32 aCharCode) const;
		size_t			GetSize		() const { return mSize; }
		size_t			GetNumGlyphs() const { return mGlyphs.size(); }

		void			EnumerateAllGlyphs(EnumerationCallback aCallback) const;

	private:
		void			ClearVars	();
		void			FreeVars	();
		
	private:
		typedef std::map<uint32,Glyph>::iterator		iterator;
		typedef std::map<uint32,Glyph>::const_iterator	const_iterator;
		
		bool					mOk;
		size_t					mSize;
		std::map<uint32,Glyph>	mGlyphs;
	};
	
	bool LoadFont(const char* aFilename, Font& aFont);
	bool SaveFont(const char* aFilename, const Font& aFont);
}
