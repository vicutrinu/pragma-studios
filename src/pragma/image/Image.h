#pragma once

#include <stddef.h>
#include <pragma/image/types.h>

namespace pragma
{
	class Image
	{
	public:
		typedef RGBPixel* iterator;

				Image		( size_t aWidth, size_t aHeight );
				~Image		();

		size_t	GetWidth	() const { return mWidth; }
		size_t	GetHeight	() const { return mHeight; }

		iterator begin		() const { return mData; }
		iterator end		() const { return &mData[mWidth*mHeight]; }

	private:
		size_t		mWidth;
		size_t		mHeight;
		RGBPixel*	mData;
	};

}
