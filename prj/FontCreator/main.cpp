#include <stdio.h>
#include "FontGenerator.h"
#include "Font.h"

int main (int argc, char * const argv[])
{
	printf("Font Creator 0.01\n");
	
	pragma::Font lFont;
	pragma::uint32 lRanges[] = { '0', '9', 'a', 'z', 'A', 'Z', 1, 0 };
	pragma::CreateFont_FreeType("/System/Library/Fonts/AppleGothic.ttf", 16, lRanges, lFont);
	
    return 0;
}


