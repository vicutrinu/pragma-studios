#pragma once

#include "internal_types.h"
#include <pragma/math/math.h>
#include "TextureSampler.h"

namespace pragma { namespace Raster
{

	template<>
	inline void RasterLines<VertexFormat::Texture,TextureRaster>( Real& aLeftStart, Real& aRightStart, unsigned& aY
																, Real aLeftIncrement, Real aRightIncrement, unsigned aCount
																, VertexFormat::Texture::ScanlineParameters::Increments& aIncrements
																, VertexFormat::Texture::ScanlineParameters::Edge& aLeft
																, VertexFormat::Texture::ScanlineParameters::Edge& aRight )
	{
		Real lLeftScan;
		Real lRightScan;
		
		while( aCount-- )
		{
			lLeftScan  = aLeftStart;
			lRightScan = aRightStart;
			
			unsigned lCount;
			Real lAdjustX = AdjustScanline(lLeftScan, lRightScan - lLeftScan, lCount);
			
			if(lCount > 0)
			{
				UV lStartUV = ((aRight.mUV - aLeft.mUV) * lAdjustX) + aLeft.mUV;
				
				unsigned lPosition = (aY * Raster::sWidth + unsigned(lLeftScan));
				unsigned char* lPtr = &sScreen[lPosition<<2];
				
				while(lCount--)
				{
					RGBA lVal = TextureSampler<TextureSampler_Debug>::Sample(lStartUV);
					*lPtr++ = lVal.x;
					*lPtr++ = lVal.y;
					*lPtr++ = lVal.z;
					*lPtr++ = 0;
					lStartUV+= aIncrements.mUVGradient;
				}
			}
			aLeftStart+= aLeftIncrement;
			aRightStart+= aRightIncrement;
			
			aLeft.mUV+= aLeft.mUVGradient;
			aRight.mUV+= aRight.mUVGradient;
			
			aY++;
		}
	}
	
	NULL_INTERPOLATOR(InterpolateColors, VertexFormat::Texture)
	NULL_ADJUST(AdjustScanlineColors, VertexFormat::Texture)
	
} }
