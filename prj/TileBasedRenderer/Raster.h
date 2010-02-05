/**
 *	@file	Raster.h
 *  @author	Victor Soria
 *
 *	@brief	Rasterization general functions
 */

#pragma once
#include <pragma/graphics/types.h>

namespace pragma
{
	namespace Raster
	{
		typedef Point  _Point;		
		typedef _Point _Vector;
		void RasterTrapezoid1(const _Point& aStart, const _Vector& aLeftEdge, const _Vector& aRightEdge);
	}
	
	/**
	 *	El color se coge de una variable de estado
	 */
	void RasterTriangle(const Point& aV0, const Point& aV1, const Point& aV2);
}
