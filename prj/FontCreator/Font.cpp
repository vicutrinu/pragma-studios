/**
 *	@file	Font.cpp
 *	@author	Victor Soria
 */

#include "Font.h"

namespace pragma
{
	
	/**
	 *	Font Constructor
	 */
	Font::Font()
	{
		ClearVars();
	}

	/**
	 *	ClearVars
	 */
	void Font::ClearVars()
	{
		mSize = 0;
		mGlyphs.clear();
	}

	/**
	 *	FreeVars
	 */
	void Font::FreeVars()
	{
		for(size_t i = 0; i < mGlyphs.size(); ++i)
		{
			free( mGlyphs[i].mBitmap );
		}
	}

	/**
	 *	Init
	 */
	void Font::Init(size_t aSize)
	{
		End();
		
		mOk = true;
		mSize = aSize;
	}

	/**
	 *	End
	 */
	void Font::End()
	{
		if(mOk)
		{
			mOk = false;
			FreeVars();
			ClearVars();
		}
	}

	/**
	 *	AddGlyph
	 */
	void Font::AddGlyph(Glyph& aGlyph)
	{
		mGlyphs[aGlyph.mCode] = aGlyph;
		iterator lIter = mGlyphs.find(aGlyph.mCode);
		lIter->second.mBitmap = (uint8*)malloc(aGlyph.mWidth * aGlyph.mHeight);
		memcpy(lIter->second.mBitmap, aGlyph.mBitmap, aGlyph.mWidth * aGlyph.mHeight);
	}
	
	/**
	 *	GetGlyph
	 */
	const Font::Glyph* Font::GetGlyph(uint32 aCharCode) const
	{
		const_iterator lIter = mGlyphs.find(aCharCode);
		if( lIter == mGlyphs.end() )
			return 0;
		else
			return &(lIter->second);
	}
	
}
