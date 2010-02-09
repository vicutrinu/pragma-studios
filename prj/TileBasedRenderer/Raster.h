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
		typedef vector2<Real> _Point2;
		typedef vector2<Real> _Vector2;
		typedef vector3<Real> _Color;

		void AddVertex(const _Point2& aPosition);
		void VertexColor(const _Color& aColor);
		void RasterTriangle		( const _Point2& aV0, const _Point2& aV1, const _Point2& aV2 );
		void SetRenderContext	( unsigned char* aBuffer, int aWidth, int aHeight );
		void ClearBackBuffer	();
		void Render				();
		
		
	}
}
