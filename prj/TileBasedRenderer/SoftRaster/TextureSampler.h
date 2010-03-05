#pragma once

#include "internal_types.h"

namespace pragma { namespace Raster
{

	template<typename T>
	class TextureSampler
	{
	};
	
	struct TextureSampler_Debug;
	
	template<>
	class TextureSampler<TextureSampler_Debug>
	{
	public:
		static inline RGBA Sample(const UV& aUV)
		{
			UV lUV( min<Real>(1, max<Real>(aUV.x, 0))
				  , min<Real>(1, max<Real>(aUV.y, 0)) ); // Clampeo 0..1
			if( (unsigned(lUV.x * 511) % 64 >= 32 ) == (unsigned(lUV.y * 511) % 64 >= 32 ) )
				return RGBA(255,255,255,255);
			else
				return RGBA(0,0,0,255);
		}
	};
	
} }
