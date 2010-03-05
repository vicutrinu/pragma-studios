#pragma once

#include "internal_types.h"

namespace pragma { namespace Raster
{
		
	template<>
	inline void RasterLines<VertexFormat::Position,SimpleRaster> ( Real& aLeftStart, Real& aRightStart, unsigned& aY
																 , Real aLeftIncrement, Real aRightIncrement, unsigned aCount
																 , VertexFormat::Position::ScanlineParameters::Increments& aIncrements
																 , VertexFormat::Position::ScanlineParameters::Edge& aLeft
																 , VertexFormat::Position::ScanlineParameters::Edge& aRight )
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
			aRightStart+= aRightIncrement;
			
			aY++;
		}
	}
	
} }
