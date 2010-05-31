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
	
	void Font::EnumerateAllGlyphs(EnumerationCallback aCallback) const
	{
		const_iterator lIter = mGlyphs.begin();
		while(lIter !=  mGlyphs.end())
		{
			aCallback(lIter->second);
			++lIter;
		}
	}
	
	/**
	 *	LoadFont
	 */
	bool LoadFont(const char* aFilename, Font& aFont)
	{
		FILE* handle = fopen(aFilename, "rb");
		if(handle == 0)
			return false;
		uint32 lVersion;
		fread(&lVersion, 1, 4, handle);
		if(lVersion != 0)
		{
			fclose(handle);
			return false;
		}
		uint32 lSize;
		fread(&lSize, 1, 4, handle);
		aFont.Init(lSize);
		uint32 lNumGlyphs;
		fread(&lNumGlyphs, 1, 4, handle);
		for(size_t i = 0; i < lNumGlyphs; ++i)
		{
			Font::Glyph lGlyph;
			fread(&lGlyph.mCode, 1, sizeof(lGlyph.mCode), handle);
			uint32 lSize;
			fread(&lSize, 1, sizeof(lSize), handle);
			if(lSize)
			{
				lGlyph.mBitmap = (uint8*)malloc(lSize);
				fread(lGlyph.mBitmap, 1, lSize, handle);
			}
			else
			{
				lGlyph.mBitmap = 0;
			}
			fread(&lGlyph.mWidth,    1, sizeof(lGlyph.mWidth),    handle);
			fread(&lGlyph.mHeight,   1, sizeof(lGlyph.mHeight),   handle);
			fread(&lGlyph.mStartX,   1, sizeof(lGlyph.mStartX),   handle);
			fread(&lGlyph.mStartY,   1, sizeof(lGlyph.mStartY),   handle);
			fread(&lGlyph.mAdvanceX, 1, sizeof(lGlyph.mAdvanceX), handle);
			aFont.AddGlyph(lGlyph);
			free(lGlyph.mBitmap);
		}
		fclose(handle);
		return true;
	}

	static FILE* sHandle = 0;
	
	static void EnumerationCallback(const Font::Glyph& aGlyph)
	{
		fwrite(&aGlyph.mCode, 1, sizeof(aGlyph.mCode), sHandle);
		if(aGlyph.mBitmap)
		{
			uint32 lSize = aGlyph.mWidth * aGlyph.mHeight;
			fwrite(&lSize, 1, sizeof(lSize), sHandle);
			fwrite(aGlyph.mBitmap, 1, lSize, sHandle);
		}
		else
		{
			uint32 lSize = 0;
			fwrite(&lSize, 1, sizeof(lSize), sHandle);
		}
		fwrite(&aGlyph.mWidth,    1, sizeof(aGlyph.mWidth),    sHandle);
		fwrite(&aGlyph.mHeight,   1, sizeof(aGlyph.mHeight),   sHandle);
		fwrite(&aGlyph.mStartX,   1, sizeof(aGlyph.mStartX),   sHandle);
		fwrite(&aGlyph.mStartY,   1, sizeof(aGlyph.mStartY),   sHandle);
		fwrite(&aGlyph.mAdvanceX, 1, sizeof(aGlyph.mAdvanceX), sHandle);
	}
	
	/**
	 *	SaveFont
	 */
	bool SaveFont(const char* aFilename, const Font& aFont)
	{
		FILE* handle = fopen(aFilename, "wb");
		if(handle == 0)
			return false;
		uint32 lVersion = 0;
		fwrite(&lVersion, 1, 4, handle);
		uint32 lSize = aFont.GetSize();
		fwrite(&lSize, 1, 4, handle);
		uint32 lNumGlyphs = aFont.GetNumGlyphs();
		fwrite(&lNumGlyphs, 1, 4, handle);

		sHandle = handle;
		aFont.EnumerateAllGlyphs(EnumerationCallback);

		sHandle = 0;
		fclose(handle);
		return true;
	}
	
}
