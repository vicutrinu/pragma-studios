#include "Image.h"
#include <pragma/image/types.h>

namespace pragma
{

	Image::Image(size_t aWidth, size_t aHeight)
		: mWidth(aWidth)
		, mHeight(aHeight)
		, mData(0)
	{
		mData = new RGBPixel[mWidth * mHeight];
	}

	Image::~Image()
	{
		delete[] mData;
	}

}
