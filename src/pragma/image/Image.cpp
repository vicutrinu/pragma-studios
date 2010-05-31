#include "Image.h"
#include <pragma/image/types.h>
#include <stdlib.h>
#include <memory.h>

namespace pragma
{

	int mierda()
	{
		return 1;
	}
	
	Image::Image(size_t aWidth, size_t aHeight)
		: mWidth(aWidth)
		, mHeight(aHeight)
		, mData(0)
	{
		mData = (RGBPixel*)malloc(sizeof(RGBPixel) * mWidth * mHeight);
		mAlphaChannel = (AlphaPixel*)malloc(sizeof(float) * mWidth * mHeight);
	}
	
	Image::Image(const Image& aImage)
		: mWidth(aImage.mWidth)
		, mHeight(aImage.mHeight)
		, mData(0)
		, mAlphaChannel(0)
	{
		mData = (RGBPixel*)malloc(sizeof(RGBPixel) * mWidth * mHeight);
		memcpy(mData, aImage.mData, sizeof(RGBPixel) * mWidth * mHeight);
		mAlphaChannel = (AlphaPixel*)malloc(sizeof(float) * mWidth * mHeight);
		memcpy(mAlphaChannel, aImage.mAlphaChannel, sizeof(float) * mWidth * mHeight);
	}
	
	Image& Image::operator=(const Image& aImage)
	{
		this->~Image();
		mWidth = aImage.mWidth;
		mHeight = aImage.mHeight;
		mData = (RGBPixel*)malloc(sizeof(RGBPixel) * mWidth * mHeight);
		memcpy(mData, aImage.mData, sizeof(RGBPixel) * mWidth * mHeight);
		mAlphaChannel = (AlphaPixel*)malloc(sizeof(float) * mWidth * mHeight);
		memcpy(mAlphaChannel, aImage.mAlphaChannel, sizeof(float) * mWidth * mHeight);
		return *this;
	}

	Image::~Image()
	{
		free(mData);
		mData = 0;
		free(mAlphaChannel);
		mAlphaChannel = 0;
	}

}
