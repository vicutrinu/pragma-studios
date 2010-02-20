#pragma once

namespace pragma { namespace Raster
{
		
	template<>
	inline void RasterLines<Draft,void>( Real& aLeftStart, Real& aRightStart, unsigned& aY
									, Real aLeftIncrement, Real aRightIncrement, unsigned aCount
									, Draft::ScanlineParameters::Increments& aIncrements
									, Draft::ScanlineParameters::Edge& aLeft
									, Draft::ScanlineParameters::Edge& aRight )
	{
		Real lLeftScan;
		Real lRightScan;
		
		while( aCount-- )
		{
			lLeftScan  = aLeftStart;
			lRightScan = aRightStart;
			
			unsigned lCount;
			AdjustScanline(lLeftScan, lRightScan - lLeftScan, lCount);
			
			if(lCount > 0)
			{
				unsigned lPosition = (aY * Raster::sWidth + unsigned(lLeftScan));
				unsigned char* lPtr = &sScreen[lPosition<<2];
				
				while(lCount--)
				{
					*lPtr++ = 255;
					*lPtr++ = 255;
					*lPtr++ = 255;
					*lPtr++ = 0;
				}
			}
			aLeftStart+= aLeftIncrement;
			aRightStart= aRightIncrement;
			
			aY++;
		}
	}
	
	NULL_INTERPOLATOR(InterpolateColors, Draft)
	NULL_INTERPOLATOR(InterpolateUVs, Draft)
	NULL_ADJUST(AdjustScanlineColors, Draft)
	NULL_ADJUST(AdjustScanlineNormals, Draft)
	NULL_ADJUST(AdjustScanlineUVs, Draft)
	
} }
