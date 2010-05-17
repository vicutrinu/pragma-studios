#pragma once

#include <pragma/types.h>
#include <vector>

namespace pragma
{
	class Font;

	/**
	 *	CreateFont_FreeType
	 *	Initialize a Font object with a font extracted using TrueType library
	 *	@param[in]	aFontFile	Name of the file that contains the font to be readed from FreeType. Usually a .ttf file
	 *	@param[in]	aSize		Size of the font to be extracted
	 *	@param[in]	aRanges		Array of unsigned pairs. Each pair contains a range of Character codes to be extracted 
	 *							from the font. The array must be ended with the first pair component greater than the second
	 *							Example: { 32, 42, 80, 100, 1, 0 }	This extracts all glyphs between 32 and 42
	 *																and between 80 and 100
	 *																The array ends with 1 > 0
	 *							If the pointer is 0, all glyps present on the font will be extracted
	 *	@param[out]	aFont		A Font object. If the object is already initialized it will be destroyed before creating the
	 *							new font
	 */
	void CreateFont_FreeType( const char* aFontFile, size_t aSize, uint32* aRanges, Font& aFont );

	/**
	 *	CreateFont_FreeType
	 *	Initialize a Font object with a font extracted using TrueType library. Extract all glyphs present on the source font
	 *	@param[in]	aFontFile	Name of the file that contains the font to be readed from FreeType. Usually a .ttf file
	 *	@param[in]	aSize		Size of the font to be extracted
	 *	@param[out]	aFont		A Font object. If the object is already initialized it will be destroyed before creating the
	 *							new font
	 */
	void CreateFont_FreeType( const char* aFontFile, size_t aSize, Font& aFont );
}
