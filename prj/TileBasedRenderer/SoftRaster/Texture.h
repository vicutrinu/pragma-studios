#pragma once

#include <pragma/math/math.h>

namespace pragma { namespace Raster
{

	static inline RGBA SampleTexture(const UV& aUV)
	{
		UV lUV( min<Real>(1, max<Real>(aUV.x, 0))
			  , min<Real>(1, max<Real>(aUV.y, 0)) ); // Clampeo 0..1
		if( (unsigned(lUV.x * 511) % 64 >= 32 ) == (unsigned(lUV.y * 511) % 64 >= 32 ) )
			return RGBA(255,255,255,255);
		else
			return RGBA(0,0,0,255);
	}
	
	template<>
	inline void RasterLines<Texture,TextureRaster>( Real& aLeftStart, Real& aRightStart, unsigned& aY
									 , Real aLeftIncrement, Real aRightIncrement, unsigned aCount
									 , Texture::ScanlineParameters::Increments& aIncrements
									 , Texture::ScanlineParameters::Edge& aLeft
									 , Texture::ScanlineParameters::Edge& aRight )
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
					RGBA lVal = SampleTexture(lStartUV);
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
	
	NULL_INTERPOLATOR(InterpolateColors, Texture)
	NULL_ADJUST(AdjustScanlineColors, Texture)
	
} }
