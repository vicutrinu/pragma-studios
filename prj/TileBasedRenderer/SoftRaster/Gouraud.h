#pragma once

#include "internal_types.h"
#include <pragma/math/math.h>

namespace pragma { namespace Raster
{

	template<>
	inline void RasterLines<VertexFormat::Normal, GouraudRaster>( Real& aLeftStart, Real& aRightStart, unsigned& aY
																, Real aLeftIncrement, Real aRightIncrement, unsigned aCount
																, VertexFormat::Normal::ScanlineParameters::Increments& aIncrements
																, VertexFormat::Normal::ScanlineParameters::Edge& aLeft
																, VertexFormat::Normal::ScanlineParameters::Edge& aRight )
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
				_Vector lStartNormal = ((aRight.mNormal - aLeft.mNormal) * lAdjustX) + aLeft.mNormal;
				
				unsigned lPosition = (aY * Raster::sWidth + unsigned(lLeftScan));
				unsigned char* lPtr = &sScreen[lPosition<<2];
				
				while(lCount--)
				{
					_Vector lVector = Normalize(lStartNormal);
					*lPtr++ = lVector.x;
					*lPtr++ = lVector.y;
					*lPtr++ = lVector.z;
					*lPtr++ = 0;
					lStartNormal+= aIncrements.mNormalGradient;
				}
			}
			aLeftStart+= aLeftIncrement;
			aRightStart+= aRightIncrement;
			
			aLeft.mNormal+= aLeft.mNormalGradient;
			aRight.mNormal+= aRight.mNormalGradient;
			
			aY++;
		}
	}
	
} }
