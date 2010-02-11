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
	static unsigned			sNumPositions = 0;
	
	// Factores para ajustar los valores interpolables al centro del pixel
	static Real				sAdjustX;
	static Real				sAdjustLeftY;
	static Real				sAdjustRightY;
	

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
			// TODO: esto no deberia pasar... Investigalo!
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
			// TODO: esto no deberia pasar... Investigalo!
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
			sAdjustX = AdjustScanline(lLeftScan, lRightScan - lLeftScan, lCount);
			
			if(lCount > 0)
			{
				_Color lStartColor = ((aColorRight - aColorLeft) * sAdjustX) + aColorLeft;
				
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

	_Point2 sPositions[1024];
	_Color  sColors[1024];

	void RasterTriangle(int i0, int i1, int i2)
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
		
		Real lVal = (sPositions[i1].y - sPositions[i0].y) / (sPositions[i2].y - sPositions[i0].y); // Valor magico que nos lleva a la "mitad" del triangulo
		
		_Vector2 lLongEdge				= sPositions[i2] - sPositions[i0];
		_Vector2 lTopShortEdge			= sPositions[i1] - sPositions[i0];
		_Vector2 lBottomShortEdge		= sPositions[i2] - sPositions[i1];
		_Vector2 lSplit					= sPositions[i0] + (lLongEdge * lVal);
		
		_Color lLongColorEdge			= sColors[i2] - sColors[i0];
		_Color lTopShortColorEdge		= sColors[i1] - sColors[i0];
		_Color lBottomShortColorEdge	= sColors[i2] - sColors[i1];
		_Color lSplitColor				= sColors[i0] + (lLongColorEdge * lVal);

		if(sPositions[i1].x < sPositions[i2].x)
		{ // 1 está a la izquierda
			Real lLeftGradient	= lTopShortEdge.x / lTopShortEdge.y;
			Real lRightGradient = lLongEdge.x / lLongEdge.y;
			
			unsigned lRowCount, lTotalRowCount;

			_Vector2 lLeftStart				= sPositions[i0];
			Real lLeftScale					= AdjustEdge(lLeftStart, lTopShortEdge, lRowCount);
			_Vector2 lRightStart			= sPositions[i0];
			Real lRightScale				= AdjustEdge(lRightStart, lLongEdge, lTotalRowCount);
			
			_Color lLeftColorStart			= sColors[i0] + (lTopShortColorEdge * lLeftScale);
			_Color lRightColorStart			= sColors[i0] + (lLongColorEdge * lRightScale);
			_Color lLeftColorGradient		= lTopShortColorEdge / lTopShortEdge.y;
			_Color lRightColorGradient		= lLongColorEdge / lLongEdge.y;
			_Color lScanlineColorGradient	= (lSplitColor - sColors[i1] ) / (lSplit.x - sPositions[i1].x);
			
			RasterLines	( lLeftStart , lLeftGradient
						, lRightStart, lRightGradient
						, lRowCount
						, lLeftColorStart, lLeftColorGradient
						, lRightColorStart, lRightColorGradient
						, lScanlineColorGradient );
			
			lLeftGradient		= lBottomShortEdge.x / lBottomShortEdge.y;
			lLeftStart			= sPositions[i1];
			lLeftScale			= AdjustEdge(lLeftStart, lBottomShortEdge, lRowCount);
			
			lLeftColorStart		= sColors[i1] + (lBottomShortColorEdge * lLeftScale);
			lLeftColorGradient	= lBottomShortColorEdge / lBottomShortEdge.y;

			RasterLines	( lLeftStart , lLeftGradient
						, lRightStart, lRightGradient
						, lRowCount
						, lLeftColorStart, lLeftColorGradient
						, lRightColorStart, lRightColorGradient
						, lScanlineColorGradient );
		}
		else
		{ // 1 está a la derecha
			Real lLeftGradient	= lLongEdge.x / lLongEdge.y;
			Real lRightGradient = lTopShortEdge.x / lTopShortEdge.y;

			unsigned lRowCount, lTotalRowCount;

			_Vector2 lLeftStart				= sPositions[i0];
			Real lLeftScale					= AdjustEdge(lLeftStart, lLongEdge, lTotalRowCount);
			_Vector2 lRightStart			= sPositions[i0];
			Real lRightScale				= AdjustEdge(lRightStart, lTopShortEdge, lRowCount);
			
			_Color lRightColorStart			= sColors[i0] + (lTopShortColorEdge * lRightScale);
			_Color lLeftColorStart			= sColors[i0] + (lLongColorEdge * lLeftScale);
			_Color lRightColorGradient		= lTopShortColorEdge / lTopShortEdge.y;
			_Color lLeftColorGradient		= lLongColorEdge / lLongEdge.y;
			_Color lScanlineColorGradient	= (sColors[i1] - lSplitColor) / (sPositions[i1].x - lSplit.x);
			
			RasterLines	( lLeftStart , lLeftGradient
						, lRightStart, lRightGradient
						, lRowCount
						, lLeftColorStart, lLeftColorGradient
						, lRightColorStart, lRightColorGradient
						, lScanlineColorGradient );
			
			lRightGradient		= lBottomShortEdge.x / lBottomShortEdge.y;
			lRightStart			= sPositions[i1];
			lRightScale			= AdjustEdge(lRightStart, lBottomShortEdge, lRowCount);
			
			lRightColorStart	= sColors[i1] + (lBottomShortColorEdge * lRightScale);
			lRightColorGradient = lBottomShortColorEdge / lBottomShortEdge.y;
			
			RasterLines	( lLeftStart , lLeftGradient
						, lRightStart, lRightGradient
						, lRowCount
						, lLeftColorStart, lLeftColorGradient
						, lRightColorStart, lRightColorGradient
						, lScanlineColorGradient );
		}
	}
	
	void RasterTriangle(const _Point2& aV0, const _Point2& aV1, const _Point2& aV2)
	{
		// Copia guarra guarra, esto deberia desaparece cuando se proporcione un interfaz mas adecuado al usuario (vertex lists)
		sPositions[0] = aV0;
		sPositions[1] = aV1;
		sPositions[2] = aV2;
		RasterTriangle(0, 1, 2);
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
			RasterTriangle(i+0, i+1, i+2);
		sNumPositions = 0;
	}

} }
