//#define _DEBUG_GENERATE_TGA_FILES_

#include "FontGenerator.h"
#include "Font.h"
#include <pragma/types.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#ifdef _DEBUG_GENERATE_TGA_FILES_
#	include <pragma/image/functions.h>
#endif

static bool			sFreeType_Initialized = false;
static FT_Library	sLibrary; 

namespace pragma
{
	
	static void ExtractGlyph(Font::Glyph& aGlyph, FT_Face aFace, uint32 aCharCode)
	{
		FT_UInt lGlyphIndex = FT_Get_Char_Index( aFace, aCharCode );
		FT_Load_Glyph(aFace, lGlyphIndex, 0);
		FT_Render_Glyph(aFace->glyph, FT_RENDER_MODE_NORMAL);
		aGlyph.mBitmap = aFace->glyph->bitmap.buffer;
		aGlyph.mWidth = aFace->glyph->bitmap.width;
		aGlyph.mHeight = aFace->glyph->bitmap.rows;
		aGlyph.mStartX = aFace->glyph->bitmap_left;
		aGlyph.mStartY = aFace->glyph->bitmap_top;
		aGlyph.mAdvanceX = aFace->glyph->advance.x;

#ifdef _DEBUG_GENERATE_TGA_FILES_
		char lNane[10];
		sprintf(lNane, "glyph%d.tga", aCharCode);
		pragma::ExportToTGA( aGlyph.mBitmap
							, aGlyph.mWidth
							, aGlyph.mHeight
							, lNane );
#endif
	}

	/**
	 *	CreateFont_FreeType
	 */
	void CreateFont_FreeType( const char* aFontFile, size_t aSize, uint32* aRanges, Font& aFont )
	{
		int error;
		// Initialize FreeType library, if needed
		if(!sFreeType_Initialized)
		{
			error = FT_Init_FreeType( &sLibrary );
			if(error)
			{
				printf( "CreateFont_FreeType: Error initializing FreeType library\n");
				return;
			}
			sFreeType_Initialized = true;			
		}

		// Initialize font face
		FT_Face lFace;
		error = FT_New_Face( sLibrary, aFontFile, 0, &lFace );
		
		if(error)
		{
			printf( "Error opening font\n");
			return;
		}

		// Set Font size
		FT_Set_Char_Size  ( lFace, 0, aSize << 6, 300, 300 );
		FT_Set_Pixel_Sizes( lFace, 0, aSize );
		
		aFont.Init(aSize);
		
		if(aRanges == 0)
		{	// Extract all glyphs
			FT_UInt lIndex;
			FT_ULong lCharCode = FT_Get_First_Char(lFace, &lIndex);
			for(size_t i = 0; i < lFace->num_glyphs; ++i)
			{
				Font::Glyph lGlyph;
				ExtractGlyph(lGlyph, lFace, lCharCode);
				aFont.AddGlyph(lGlyph);
				lCharCode = FT_Get_Next_Char(lFace, lCharCode, &lIndex);
			}
		}
		else
		{	// Extract only a range of glyphs
			while(aRanges[0] <= aRanges[1])
			{
				for(uint32 i = aRanges[0]; i <= aRanges[1]; ++i)
				{
					Font::Glyph lGlyph;
					ExtractGlyph(lGlyph, lFace, i);
					aFont.AddGlyph(lGlyph);
				}
				aRanges+=2;
			}
		}

	}

	/**
	 *	CreateFont_FreeType
	 */
	void CreateFont_FreeType( const char* aFontFile, size_t aSize, Font& aFont )
	{
		CreateFont_FreeType( aFontFile, aSize, 0, aFont);
	}
}
