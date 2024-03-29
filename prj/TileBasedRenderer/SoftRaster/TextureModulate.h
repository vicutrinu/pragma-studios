#pragma once

#include "internal_types.h"
#include <pragma/math/math.h>
#include "TextureSampler.h"

namespace pragma { namespace Raster
{

	template<>
	inline void RasterLines<VertexFormat::Color_Texture,TextureModulateRaster>( Real& aLeftStart, Real& aRightStart, unsigned& aY
																			  , Real aLeftIncrement, Real aRightIncrement, unsigned aCount
																			  , VertexFormat::Color_Texture::ScanlineParameters::Increments& aIncrements
																			  , VertexFormat::Color_Texture::ScanlineParameters::Edge& aLeft
																			  , VertexFormat::Color_Texture::ScanlineParameters::Edge& aRight )
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
				_Color lColorVal = ((aRight.mColor - aLeft.mColor) * lAdjustX) + aLeft.mColor;
				
				unsigned lPosition = (aY * Raster::sWidth + unsigned(lLeftScan));
				unsigned char* lPtr = &sScreen[lPosition<<2];

				while(lCount--)
				{
					RGBA lVal = TextureSampler<TextureSampler_Debug>::Sample(lStartUV);
					*lPtr++ = unsigned((float)lVal.x * (float)lColorVal.x / 255.f);
					*lPtr++ = unsigned((float)lVal.y * (float)lColorVal.y / 255.f);
					*lPtr++ = unsigned((float)lVal.z * (float)lColorVal.z / 255.f);
					*lPtr++ = 0;
					lStartUV+= aIncrements.mUVGradient;
					lColorVal+= aIncrements.mColorGradient;
				}
			}
			aLeftStart+= aLeftIncrement;
			aRightStart+= aRightIncrement;
			
			aLeft.mUV+= aLeft.mUVGradient;
			aRight.mUV+= aRight.mUVGradient;
			
			aLeft.mColor+= aLeft.mColorGradient;
			aRight.mColor+= aRight.mColorGradient;
			
			aY++;
		}
	}
	
} }
