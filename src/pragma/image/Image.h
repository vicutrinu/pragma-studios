#pragma once

#include <pragma/types.h>
#include <pragma/image/types.h>

namespace pragma
{
	class Image
	{
	public:
		typedef RGBPixel*	iterator;
		typedef float*		alpha_iterator;


				Image		( size_t aWidth, size_t aHeight );
				Image		( const Image& aImage );
				~Image		();
		
		Image&	operator=	(const Image& aImage);

		size_t	GetWidth	() const { return mWidth; }
		size_t	GetHeight	() const { return mHeight; }

		iterator begin		() const { return mData; }
		iterator end		() const { return &mData[mWidth*mHeight]; }
		
		alpha_iterator	alpha_begin	() const { return mAlphaChannel; }
		alpha_iterator	alpha_end	() const { return &mAlphaChannel[mWidth*mHeight]; }

	private:
		size_t		mWidth;
		size_t		mHeight;
		RGBPixel*	mData;
		AlphaPixel* mAlphaChannel;
	};

}
