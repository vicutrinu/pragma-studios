#include <stdio.h>
#include <pragma/pragma.h>
#include "FontGenerator.h"
#include "Font.h"
#include "TextBitmap.h"

#include <pragma/image/functions.h>

int main (int argc, char * const argv[])
{
	printf("Font Creator 0.01\n");
	
	pragma::Font lFont;
	/*pragma::uint32 lRanges[] = { ' ', ' ', '0', '9', 'a', 'z', 'A', 'Z', 1, 0 };
	pragma::CreateFont_FreeType("/Library/Fonts/Arial.ttf", 500, 0, lFont);
	printf("%d Characters\n", lFont.GetNumGlyphs());
	pragma::SaveFont("Fuentuca.font", lFont);
	lFont.End();*/
	
	pragma::LoadFont("Fuentuca.font", lFont);
	const char lText[] = "Me molo a mi mismo. No te digo trigo por no llamarte Rodrigo. Maria de la O, que desgraciaita gitana tu eres, teniendolo to...";
	pragma::vector2f lSize(512,512);
	pragma::vector2f lStart;
	CreateTextBitmap(lFont, lText, 0, lSize, lStart);
	pragma::uint8* lBitmap = (pragma::uint8*)malloc(lSize.x * lSize.y);
	pragma::vector2f lTextSize = lSize;
	CreateTextBitmap(lFont, lText, lBitmap, lTextSize, lStart);
	pragma::ExportToTGA( lBitmap, lSize.x, lSize.y, "out.tga" );
	
    return 0;
}


