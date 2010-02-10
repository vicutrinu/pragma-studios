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
	
	inline void RasterLines( _Point2 aLeftStart, Real aLeftIncrement
						   , _Point2 aRightStart, Real aRightIncrement
						   , unsigned aCount
						   , _Color aColorLeft, _Color aLeftColorInc
						   , _Color aColorRight, _Color aRightColorInc
						   , _Color aScanlineIncColor )
	{
		Real lLeftScan;
		Real lRightScan;
		unsigned lY = unsigned(aLeftStart.y);
		while( aCount-- )
		{
			lLeftScan  = aLeftStart.x;
			lRightScan = aRightStart.x;
			
			unsigned lCount;
			sAdjustX = AdjustScanline(lLeftScan, lRightScan - lLeftScan, lCount);
			
			if(lCount > 0)
			{
				_Color lStartColor = ((aColorRight - aColorLeft) * sAdjustX) + aColorLeft;
				//_Color lIncColor = (aColorRight - aColorLeft) / (lRightScan.x - lLeftScan.x);
				
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

	inline void RasterTopTriangle( const _Point2& aStart, const _Vector2& aLeftEdge, const _Vector2& aRightEdge
								 , const _Color& aColorStart, const _Color& aLeftColorEdge, const _Color& aRightColorEdge )
	{
		// Interpolamos el lado izquierdo
		_Point2 lLeftStart = aStart;
		unsigned lLeftRowCount;
		sAdjustLeftY = AdjustEdge(lLeftStart, aLeftEdge, lLeftRowCount);
		
		// Interpolamos el lado derecho
		_Point2 lRightStart = aStart;
		unsigned lRightRowCount;
		sAdjustRightY = AdjustEdge(lRightStart, aRightEdge, lRightRowCount);
		if(lLeftRowCount != lRightRowCount)
			return;
		
		if(lLeftRowCount == 0)
			return;

		_Color lColorLeft = aColorStart + (aLeftColorEdge * sAdjustLeftY);
		_Color lColorRight = aColorStart + (aRightColorEdge * sAdjustRightY);
		_Color lLeftColorInc = aLeftColorEdge / aLeftEdge.y;
		_Color lRightColorInc = aRightColorEdge / aRightEdge.y;
		_Color lScanlineIncColor = (aRightColorEdge - aLeftColorEdge) / (aRightEdge.x - aLeftEdge.x);
		
		RasterLines( lLeftStart , aLeftEdge.x  / aLeftEdge.y
				   , lRightStart, aRightEdge.x / aRightEdge.y
				   , lLeftRowCount
				   , lColorLeft, lLeftColorInc
				   , lColorRight, lRightColorInc
				   , lScanlineIncColor );
	}
		
	inline void RasterBottomTriangle(const _Point2& aStart, const _Vector2& aLeftEdge, const _Vector2& aRightEdge
									, const _Color& aColorStart, const _Color& aLeftColorEdge, const _Color& aRightColorEdge )
	{
		// Interpolamos el lado izquierdo
		_Point2 lLeftStart = aStart + aLeftEdge;
		unsigned lLeftRowCount;
		sAdjustLeftY = AdjustEdge(lLeftStart, -aLeftEdge, lLeftRowCount);
		
		// Interpolamos el lado derecho
		_Point2 lRightStart = aStart + aRightEdge;
		unsigned lRightRowCount;
		sAdjustRightY = AdjustEdge(lRightStart, -aRightEdge, lRightRowCount);
		
		if(lLeftRowCount != lRightRowCount)
			return;
		
		if(lLeftRowCount == 0)
			return;
		
		_Color lColorLeft = (aColorStart + aLeftColorEdge) - (aLeftColorEdge * sAdjustLeftY);
		_Color lColorRight = (aColorStart + aRightColorEdge) - (aRightColorEdge * sAdjustRightY);
		_Color lLeftColorInc = aLeftColorEdge / aLeftEdge.y;
		_Color lRightColorInc = aRightColorEdge / aRightEdge.y;
		_Color lScanlineIncColor = (aRightColorEdge - aLeftColorEdge) / (aRightEdge.x - aLeftEdge.x);
		
		RasterLines( lLeftStart , aLeftEdge.x  / aLeftEdge.y
				   , lRightStart, aRightEdge.x / aRightEdge.y
				   , lLeftRowCount
				   , lColorLeft, lLeftColorInc
				   , lColorRight, lRightColorInc
				   , lScanlineIncColor );
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
		
		_Vector2 lEdge = sPositions[i2] - sPositions[i0];
		Real lVal = (sPositions[i1].y - sPositions[i0].y) / (lEdge.y); // Valor magico
		_Vector2 lSplit = sPositions[i0] + (lEdge * lVal); 
		if(sPositions[i1].x < sPositions[i2].x)
		{ // 1 está a la izquierda
			RasterTopTriangle   ( sPositions[i0], sPositions[i1] - sPositions[i0], lSplit - sPositions[i0]
								, sColors[i0],    sColors[i1] - sColors[i0],       sColors[i0] + ((sColors[i2] - sColors[i0]) * lVal) - sColors[i0] );
			RasterBottomTriangle( sPositions[i2], sPositions[i1] - sPositions[i2], lSplit - sPositions[i2]
								, sColors[i2],    sColors[i1] - sColors[i2],       sColors[i0] + ((sColors[i2] - sColors[i0]) * lVal) - sColors[i2] );
		}
		else
		{ // 1 está a la derecha
			RasterTopTriangle   ( sPositions[i0], lSplit - sPositions[i0], sPositions[i1] - sPositions[i0]
								, sColors[i0],    sColors[i0] + ((sColors[i2] - sColors[i0]) * lVal) - sColors[i0], sColors[i1] - sColors[i0] );
			RasterBottomTriangle( sPositions[i2], lSplit - sPositions[i2], sPositions[i1] - sPositions[i2]
								, sColors[i2],    sColors[i0] + ((sColors[i2] - sColors[i0]) * lVal) - sColors[i2], sColors[i1] - sColors[i2] );
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
