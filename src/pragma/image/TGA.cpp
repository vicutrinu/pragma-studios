#include <pragma/types.h>
#include <pragma/image/functions.h>
#include <pragma/image/Image.h>
#include <pragma/math/math.h>

#include <stdio.h>

namespace pragma
{
	namespace impl
	{
		// this struct was taken from http://www.organicbit.com/closecombat/formats/tga.html
		// more info on http://en.wikipedia.org/wiki/Truevision_TGA
		#pragma pack(1)
		struct TGAHeader
		{
			uint8	identsize;			// size of ID field that follows 18 byte header (0 usually)
			uint8	colourmaptype;		// type of colour map 0=none, 1=has palette
			uint8	imagetype;			// type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed

			uint16	colourmapstart;		// first colour map entry in palette
			uint16	colourmaplength;	// number of colours in palette
			uint8	colourmapbits;		// number of bits per palette entry 15,16,24,32

			uint16	xstart;				// image x origin
			uint16	ystart;				// image y origin
			uint16	width;				// image width in pixels
			uint16	height;				// image height in pixels
			uint8	bits;				// image bits per pixel 8,16,24,32
			uint8	descriptor;			// image descriptor bits (vh flip bits)
		};
		#pragma pack()
	}

	bool ExportToTGA(const Image& aImage, const char* aFilename)
	{
		FILE* handle = fopen(aFilename, "wb");

		if(handle == 0)
			return false;

		impl::TGAHeader lHeader;
		lHeader.identsize		= 0; // no data between header and image raw
		lHeader.colourmaptype	= 0; // no color map
		lHeader.imagetype		= 2; // RGB
		lHeader.colourmapstart	= 0;
		lHeader.colourmaplength = 0;
		lHeader.colourmapbits	= 0; // 24 bpp
		lHeader.xstart			= 0;
		lHeader.ystart			= 0;
		lHeader.width			= uint16(aImage.GetWidth());
		lHeader.height			= uint16(aImage.GetHeight());
		lHeader.bits			= 24; // 24 bpp
		lHeader.descriptor		= 1<<5; // bits 3-0 give the alpha channel depth, bits 5-4 give direction

		fwrite(&lHeader, 1, sizeof(lHeader), handle);

		Image::iterator lIter = aImage.begin();
		while(lIter != aImage.end() )
		{
			vector3<uint8> lRGB8;
			lRGB8.i[0] = uint8( clamp<ColorIntensity>(lIter->i[2], 0, 1) * 255 );
			lRGB8.i[1] = uint8( clamp<ColorIntensity>(lIter->i[1], 0, 1) * 255 );
			lRGB8.i[2] = uint8( clamp<ColorIntensity>(lIter->i[0], 0, 1) * 255 );
			++lIter;
			fwrite(&lRGB8, 1, sizeof(lRGB8), handle);
		}

		fclose(handle);
		return true;
	}

	bool ExportToTGA(const uint8* aImage, size_t aWidth, size_t aHeight, const char* aFilename)
	{
		FILE* handle = fopen(aFilename, "wb");
		
		if(handle == 0)
			return false;
		
		impl::TGAHeader lHeader;
		lHeader.identsize		= 0; // no data between header and image raw
		lHeader.colourmaptype	= 0; // no color map
		lHeader.imagetype		= 3; // Grey scale
		lHeader.colourmapstart	= 0;
		lHeader.colourmaplength = 0;
		lHeader.colourmapbits	= 0; // 8 bpp
		lHeader.xstart			= 0;
		lHeader.ystart			= 0;
		lHeader.width			= uint16(aWidth);
		lHeader.height			= uint16(aHeight);
		lHeader.bits			= 8; // 8 bpp
		lHeader.descriptor		= 1<<5; // bits 3-0 give the alpha channel depth, bits 5-4 give direction
		
		fwrite(&lHeader, 1, sizeof(lHeader), handle);

		size_t lNumPixels = aWidth * aHeight;
		while(lNumPixels)
		{
			fwrite(aImage, 1, sizeof(uint8), handle);
			lNumPixels--;
			aImage++;
		}
		
		fclose(handle);
		return true;
	}

	/*bool ImportFromTGA(Image& aImage, const char* aFilename)
	{
		FILE* handle = fopen(aFilename, "rb");

		if(handle == 0)
			return false;

		impl::TGAHeader lHeader;

		fread(&lHeader, 1, sizeof(lHeader), handle);

		fclose(handle);
		return true;
	}
	*/
}
