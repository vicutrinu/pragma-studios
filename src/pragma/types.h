#pragma once

#include <string>

namespace pragma
{

	typedef unsigned int	uint32;
	typedef unsigned short	uint16;
	typedef unsigned char	uint8;
	typedef unsigned char   uchar;
	typedef unsigned int    uint;
	typedef unsigned int    size_t;

	template<typename T>
	struct RectSize
	{
			RectSize	() { }
			RectSize	( int aWidth, int aHeight )
						: mWidth(aWidth), mHeight(aHeight) { }
		int	mWidth;
		int	mHeight;
	};
	
	typedef RectSize<float>	RectSizef;
	typedef RectSize<int>	RectSizei;
	typedef std::string		string;
	
}
