#pragma once

#include "internal_types.h"

namespace pragma { namespace Raster
{

	template<>
	inline void RasterLines<VertexFormat::Color,VertexColorRaster>( Real& aLeftStart, Real& aRightStart, unsigned& aY
																  , Real aLeftIncrement, Real aRightIncrement, unsigned aCount
																  , VertexFormat::Color::ScanlineParameters::Increments& aIncrements
																  , VertexFormat::Color::ScanlineParameters::Edge& aLeft
																  , VertexFormat::Color::ScanlineParameters::Edge& aRight )
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
				_Color lStartColor = ((aRight.mColor - aLeft.mColor) * lAdjustX) + aLeft.mColor;
				
				unsigned lPosition = (aY * Raster::sWidth + unsigned(lLeftScan));
				unsigned char* lPtr = &sScreen[lPosition<<2];
				
				while(lCount--)
				{
					*lPtr++ = lStartColor.x;
					*lPtr++ = lStartColor.y;
					*lPtr++ = lStartColor.z;
					*lPtr++ = 0;
					lStartColor+= aIncrements.mColorGradient; //lIncColor;
				}
			}
			aLeftStart+= aLeftIncrement;
			aRightStart+= aRightIncrement;
			
			aLeft.mColor+= aLeft.mColorGradient;
			aRight.mColor+= aRight.mColorGradient;
			
			aY++;
		}
	}

} }