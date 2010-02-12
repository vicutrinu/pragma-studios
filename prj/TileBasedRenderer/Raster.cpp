/**
 *	@file	Raster.cpp
 *  @author	Victor Soria
 *
 *	@brief	Rasterization general functions
 */

#include "Raster.h"
#include <pragma/graphics/types.h>
#include <pragma/geometry/functions.h>
#include <pragma/math/functions.h>

#include <stdio.h>
#include <memory.h>
#include <assert.h>

namespace pragma { namespace Raster
{
	static unsigned			sHeight;
	static unsigned			sWidth;
	static unsigned char*	sScreen;
	
	static _Color			sColor;
	static _Point2			sPositions[1024];
	static _Color			sColors[1024];
	static unsigned			sNumPositions = 0;
	
	/**
	 *	Ajusta los extremos de un costado para centrarlo en un pixel
	 *	El ajuste se realiza desplazando el extremo hacia el siguiente centro de pixel (hacia abajo)
	 *	Ejemplo: Si el extremo se encuentra en la posicion Y=0.3 se ajusta a la posicion 0.5
	 *			 Si el extremo se encuentra en la posicion Y=0.7 se ajusta a la posicion 1.5
	 *	@param	aStart[in]	Extremo superior del costado
	 *	@param  aEdge[in]	Vector de direccion del costado, sin normalizar. aStart + aEdge es el extremo inferior
	 *	@return				Factor de avance en la direccion del costado que ajusta el extremo superior a la nueva posicion
	 *							este mismo factor se puede utilizar para ajustar otros valores del vertice (color, Z, uv, etc)
	 *  @param  aStart[out]	Extremo superior ajustado al centro de un pixel (en altura)
	 *			aCount[out]	Numero de filas que cubre el costado ya ajustado
	 */
	inline Real AdjustEdge(_Point2& aStart, const _Vector2& aEdge, unsigned& aCount)
	{
		if(aEdge.y <= 0)
		{
			aCount = 0;
			return 0;
		}
		Real lY = int(aStart.y + Real(0.5)) + Real(0.5);
		Real lVal = (lY - aStart.y) / aEdge.y; // Valor magico
		_Point2 lStart = aStart + (aEdge * lVal);
		
		Real lEndY = aStart.y + aEdge.y;
		lEndY = int(lEndY + Real(0.5)) + Real(0.5);
		
		aStart = lStart;
		aCount = unsigned(lEndY - lStart.y);
		return lVal;
	}

	/**
	 *	Ajusta los extremos de una Scanline para centrarla en un pixel
	 *	El ajuste se realiza desplazando el extremo hacia el siguiente centro de pixel (hacia la derecha)
	 *	Ejemplo: Si el extremo se encuentra en la posicion X=0.3 se ajusta a la posicion 0.5
	 *			 Si el extremo se encuentra en la posicion X=0.7 se ajusta a la posicion 1.5
	 *	@param	aStart[in]	Extremo izquierdo del costado
	 *	@param  aLength[in]	Longitud de la Scanline. aStart + aLength es el extremo derecho
	 *	@return				Factor de avance en la direccion del costado que ajusta el extremo izquierdo a la nueva posicion
	 *							este mismo factor se puede utilizar para ajustar otros valores del vertice (color, Z, uv, etc)
	 *  @param  aStart[out]	Extremo izquierdo ajustado al centro de un pixel
	 *			aCount[out]	Numero de columnas que cubre el costado ya ajustado
	 */
	inline Real AdjustScanline(Real& aStart, Real aLength, unsigned& aCount)
	{
		if(aLength <= 0)
		{
			aCount = 0;
			return 0;
		}
		Real lX = int(aStart + Real(0.5)) + Real(0.5);
		Real lVal = (lX - aStart) / aLength; // Valor magico
		Real lStart = aStart + (aLength * lVal);
		
		Real lEndX = aStart + aLength;
		lEndX = int(lEndX + Real(0.5)) + Real(0.5);
		
		aStart = lStart;
		aCount = unsigned(lEndX - lStart);
		return lVal;
	}
	
	inline void RasterLines( _Point2& aLeftStart, Real aLeftIncrement
						   , _Point2& aRightStart, Real aRightIncrement
						   , unsigned aCount
						   , _Color& aColorLeft, _Color aLeftColorInc
						   , _Color& aColorRight, _Color aRightColorInc
						   , _Color aScanlineIncColor )
	{
		Real lLeftScan;
		Real lRightScan;
		unsigned lY = unsigned(aLeftStart.y);
		aLeftStart.y+= aCount;
		
		while( aCount-- )
		{
			lLeftScan  = aLeftStart.x;
			lRightScan = aRightStart.x;
			
			unsigned lCount;
			Real lAdjustX = AdjustScanline(lLeftScan, lRightScan - lLeftScan, lCount);
			
			if(lCount > 0)
			{
				_Color lStartColor = ((aColorRight - aColorLeft) * lAdjustX) + aColorLeft;
				
				unsigned lPosition = (lY * Raster::sWidth + unsigned(lLeftScan));
				unsigned char* lPtr = &sScreen[lPosition<<2];

				while(lCount--)
				{
					*lPtr++ = lStartColor.x * 255;
					*lPtr++ = lStartColor.y * 255;
					*lPtr++ = lStartColor.z * 255;
					*lPtr++ = 0;
					lStartColor+= aScanlineIncColor; //lIncColor;
				}
			}
			aLeftStart.x+= aLeftIncrement;
			aRightStart.x+= aRightIncrement;
			
			aColorLeft+= aLeftColorInc;
			aColorRight+= aRightColorInc;
			
			lY++;
		}
	}
	
	inline void RasterLines( _Point2& aLeftStart, Real aLeftIncrement
							, _Point2& aRightStart, Real aRightIncrement
							, unsigned aCount )
	{
		Real lLeftScan;
		Real lRightScan;
		unsigned lY = unsigned(aLeftStart.y);
		aLeftStart.y+= aCount;
		
		while( aCount-- )
		{
			lLeftScan  = aLeftStart.x;
			lRightScan = aRightStart.x;
			
			unsigned lCount;
			AdjustScanline(lLeftScan, lRightScan - lLeftScan, lCount);
			
			if(lCount > 0)
			{
				unsigned lPosition = (lY * Raster::sWidth + unsigned(lLeftScan));
				unsigned char* lPtr = &sScreen[lPosition<<2];
				
				while(lCount--)
				{
					*lPtr++ = 255;
					*lPtr++ = 255;
					*lPtr++ = 255;
					*lPtr++ = 0;
				}
			}
			aLeftStart.x+= aLeftIncrement;
			aRightStart.x+= aRightIncrement;
			
			lY++;
		}
	}

	template<typename T>
	void _RasterTriangle(int i0, int i1, int i2, typename T::Interpolators& aTable);

	struct InterpolatorsBase
	{
		// Interpoladores de posicion, comunes para todos los sabores de raster
		_Vector2 mLongEdge;
		_Vector2 mTopShortEdge;
		_Vector2 mBottomShortEdge;
		_Vector2 mSplit;
	};
	
	struct Draft
	{
		struct Interpolators : public InterpolatorsBase
		{
		};
	};
	
	struct ColorVertex
	{
		struct Interpolators : public InterpolatorsBase
		{
			// Interpoladores de color
			_Color mLongColorEdge;
			_Color mTopShortColorEdge;
			_Color mBottomShortColorEdge;
			_Color mSplitColor;
		};
	};

	template<>
	inline void _RasterTriangle<Draft>(int i0, int i1, int i2, Draft::Interpolators& aTable)
	{
		if(sPositions[i1].x < sPositions[i2].x)
		{ // 1 está a la izquierda
			Real lLeftGradient	= aTable.mTopShortEdge.x / aTable.mTopShortEdge.y;
			Real lRightGradient = aTable.mLongEdge.x / aTable.mLongEdge.y;
			
			unsigned lRowCount, lTotalRowCount;
			
			_Vector2 lLeftStart		= sPositions[i0];
			Real lLeftScale			= AdjustEdge(lLeftStart, aTable.mTopShortEdge, lRowCount);
			_Vector2 lRightStart	= sPositions[i0];
			AdjustEdge(lRightStart, aTable.mLongEdge, lTotalRowCount);
			
			RasterLines	( lLeftStart , lLeftGradient, lRightStart, lRightGradient, lRowCount );
			
			lLeftGradient	= aTable.mBottomShortEdge.x / aTable.mBottomShortEdge.y;
			lLeftStart		= sPositions[i1];
			lLeftScale		= AdjustEdge(lLeftStart, aTable.mBottomShortEdge, lRowCount);
			
			RasterLines	( lLeftStart , lLeftGradient, lRightStart, lRightGradient, lRowCount );
		}
		else
		{ // 1 está a la derecha
			Real lLeftGradient	= aTable.mLongEdge.x / aTable.mLongEdge.y;
			Real lRightGradient = aTable.mTopShortEdge.x / aTable.mTopShortEdge.y;
			
			unsigned lRowCount, lTotalRowCount;
			
			_Vector2 lLeftStart		= sPositions[i0];
			AdjustEdge(lLeftStart, aTable.mLongEdge, lTotalRowCount);
			_Vector2 lRightStart	= sPositions[i0];
			Real lRightScale		= AdjustEdge(lRightStart, aTable.mTopShortEdge, lRowCount);
			
			RasterLines	( lLeftStart , lLeftGradient, lRightStart, lRightGradient, lRowCount );
			
			lRightGradient	= aTable.mBottomShortEdge.x / aTable.mBottomShortEdge.y;
			lRightStart		= sPositions[i1];
			lRightScale		= AdjustEdge(lRightStart, aTable.mBottomShortEdge, lRowCount);
			
			RasterLines	( lLeftStart , lLeftGradient, lRightStart, lRightGradient, lRowCount );
		}
	}
	
	template<>
	inline void _RasterTriangle<ColorVertex>(int i0, int i1, int i2, ColorVertex::Interpolators& aTable)
	{
		if(sPositions[i1].x < sPositions[i2].x)
		{ // 1 está a la izquierda
			Real lLeftGradient	= aTable.mTopShortEdge.x / aTable.mTopShortEdge.y;
			Real lRightGradient = aTable.mLongEdge.x / aTable.mLongEdge.y;
			
			unsigned lRowCount, lTotalRowCount;

			_Vector2 lLeftStart		= sPositions[i0];
			Real lLeftScale			= AdjustEdge(lLeftStart, aTable.mTopShortEdge, lRowCount);
			_Vector2 lRightStart	= sPositions[i0];
			Real lRightScale		= AdjustEdge(lRightStart, aTable.mLongEdge, lTotalRowCount);
			
			_Color lLeftColorStart			= sColors[i0] + (aTable.mTopShortColorEdge * lLeftScale);
			_Color lRightColorStart			= sColors[i0] + (aTable.mLongColorEdge * lRightScale);
			_Color lLeftColorGradient		= aTable.mTopShortColorEdge / aTable.mTopShortEdge.y;
			_Color lRightColorGradient		= aTable.mLongColorEdge / aTable.mLongEdge.y;
			_Color lScanlineColorGradient	= (aTable.mSplitColor - sColors[i1] ) / (aTable.mSplit.x - sPositions[i1].x);
			
			RasterLines	( lLeftStart , lLeftGradient, lRightStart, lRightGradient, lRowCount
						, lLeftColorStart, lLeftColorGradient, lRightColorStart, lRightColorGradient, lScanlineColorGradient );
			
			lLeftGradient	= aTable.mBottomShortEdge.x / aTable.mBottomShortEdge.y;
			lLeftStart		= sPositions[i1];
			lLeftScale		= AdjustEdge(lLeftStart, aTable.mBottomShortEdge, lRowCount);
			
			lLeftColorStart		= sColors[i1] + (aTable.mBottomShortColorEdge * lLeftScale);
			lLeftColorGradient	= aTable.mBottomShortColorEdge / aTable.mBottomShortEdge.y;

			RasterLines	( lLeftStart , lLeftGradient, lRightStart, lRightGradient, lRowCount
						, lLeftColorStart, lLeftColorGradient, lRightColorStart, lRightColorGradient, lScanlineColorGradient );
		}
		else
		{ // 1 está a la derecha
			Real lLeftGradient	= aTable.mLongEdge.x / aTable.mLongEdge.y;
			Real lRightGradient = aTable.mTopShortEdge.x / aTable.mTopShortEdge.y;

			unsigned lRowCount, lTotalRowCount;

			_Vector2 lLeftStart		= sPositions[i0];
			Real lLeftScale			= AdjustEdge(lLeftStart, aTable.mLongEdge, lTotalRowCount);
			_Vector2 lRightStart	= sPositions[i0];
			Real lRightScale		= AdjustEdge(lRightStart, aTable.mTopShortEdge, lRowCount);
			
			_Color lRightColorStart			= sColors[i0] + (aTable.mTopShortColorEdge * lRightScale);
			_Color lLeftColorStart			= sColors[i0] + (aTable.mLongColorEdge * lLeftScale);
			_Color lRightColorGradient		= aTable.mTopShortColorEdge / aTable.mTopShortEdge.y;
			_Color lLeftColorGradient		= aTable.mLongColorEdge / aTable.mLongEdge.y;
			_Color lScanlineColorGradient	= (sColors[i1] - aTable.mSplitColor) / (sPositions[i1].x - aTable.mSplit.x);
			
			RasterLines	( lLeftStart , lLeftGradient, lRightStart, lRightGradient, lRowCount
						, lLeftColorStart, lLeftColorGradient, lRightColorStart, lRightColorGradient, lScanlineColorGradient );
			
			lRightGradient	= aTable.mBottomShortEdge.x / aTable.mBottomShortEdge.y;
			lRightStart		= sPositions[i1];
			lRightScale		= AdjustEdge(lRightStart, aTable.mBottomShortEdge, lRowCount);
			
			lRightColorStart	= sColors[i1] + (aTable.mBottomShortColorEdge * lRightScale);
			lRightColorGradient = aTable.mBottomShortColorEdge / aTable.mBottomShortEdge.y;
			
			RasterLines	( lLeftStart , lLeftGradient, lRightStart, lRightGradient, lRowCount
						, lLeftColorStart, lLeftColorGradient, lRightColorStart, lRightColorGradient, lScanlineColorGradient );
		}
	}
	
	template<typename T>
	inline void InterpolateColors(int i0, int i1, int i2, Real aVal, typename T::Interpolators& aTable) { }
	
	template<>
	inline void InterpolateColors<ColorVertex>(int i0, int i1, int i2, Real aVal, ColorVertex::Interpolators& aTable)
	{
		aTable.mLongColorEdge			= sColors[i2] - sColors[i0];
		aTable.mTopShortColorEdge		= sColors[i1] - sColors[i0];
		aTable.mBottomShortColorEdge	= sColors[i2] - sColors[i1];
		aTable.mSplitColor				= sColors[i0] + (aTable.mLongColorEdge * aVal);
	}

	template<typename T>
	inline void RasterTriangle(int i0, int i1, int i2)
	{
		int lTemp;
		if(sPositions[i0].y >= sPositions[i1].y)
		{
			lTemp = i0;
			i0 = i1;
			i1 = lTemp;
		}
		if(sPositions[i1].y >= sPositions[i2].y)
		{
			lTemp = i1;
			i1 = i2;
			i2 = lTemp;
		}
		if(sPositions[i0].y >= sPositions[i1].y)
		{
			lTemp = i0;
			i0 = i1;
			i1 = lTemp;
		}
		
		typename T::Interpolators lTable;		

		Real lVal = (sPositions[i1].y - sPositions[i0].y) / (sPositions[i2].y - sPositions[i0].y); // Valor magico que nos lleva a la "mitad" del triangulo

		lTable.mLongEdge			= sPositions[i2] - sPositions[i0];
		lTable.mTopShortEdge		= sPositions[i1] - sPositions[i0];
		lTable.mBottomShortEdge		= sPositions[i2] - sPositions[i1];
		lTable.mSplit				= sPositions[i0] + (lTable.mLongEdge * lVal);

		InterpolateColors<T>(i0, i1, i2, lVal, lTable );
		
		_RasterTriangle<T>(i0, i1, i2, lTable);
	}
	
	void AddVertex(const _Point2& aPosition)
	{
		sPositions[sNumPositions] = aPosition;
		sColors[sNumPositions++] = sColor;
	}
	
	void VertexColor(const _Color& aColor)
	{
		sColor = aColor;
	}
	
	void SetRenderContext(unsigned char* aBuffer, int aWidth, int aHeight)
	{
		Raster::sScreen	= aBuffer;
		Raster::sWidth	= aWidth;
		Raster::sHeight	= aHeight;
	}
	
	void ClearBackBuffer()
	{
		memset(Raster::sScreen, 32, Raster::sWidth * Raster::sHeight * 4);
	}
	
	void Render()
	{
		for(unsigned i = 0; i < sNumPositions; i+= 3)
		{
			RasterTriangle<ColorVertex>(i+0, i+1, i+2);
		}
		for(unsigned i = 0; i < sNumPositions; i+= 3)
		{
			RasterTriangle<Draft>(i+0, i+1, i+2);
			break;
		}
		sNumPositions = 0;
	}

} }
