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

namespace pragma { namespace Raster
{
	static unsigned			sHeight;
	static unsigned			sWidth;
	static unsigned char*	sScreen;
	static unsigned char	sColor;
	
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
			return 0;
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
			return 0;
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
						   , _Color aColorRight, _Color aRightColorInc )
	{
		_Point2 lLeftScan;
		_Point2 lRightScan;
		while( aCount-- )
		{
			lLeftScan  = aLeftStart;
			lRightScan = aRightStart;
			
			unsigned lCount;
			sAdjustX = AdjustScanline(lLeftScan.x, lRightScan.x - lLeftScan.x, lCount);
			
			_Color lStartColor = ((aColorRight - aColorLeft) * sAdjustX) + aColorLeft;
			_Color lIncColor = (aColorRight - aColorLeft) / lCount;
			
			unsigned lPosition = (int(lLeftScan.y) * Raster::sWidth + int(lLeftScan.x));
			unsigned char* lPtr = &sScreen[(lPosition<<1) + lPosition];
			while(lCount--)
			{
				*lPtr++ = lStartColor.x * 255;
				*lPtr++ = lStartColor.y * 255;
				*lPtr++ = lStartColor.z * 255;
				lStartColor+= lIncColor;
			}
			
			aLeftStart.x+= aLeftIncrement;
			aLeftStart.y+= 1;
			aRightStart.x+= aRightIncrement;
			aRightStart.y+= 1;
			
			aColorLeft+= aLeftColorInc;
			aColorRight+= aRightColorInc;
		}
	}

	inline void RasterTopTriangle( const _Point2& aStart, const _Vector2& aLeftEdge, const _Vector2& aRightEdge
								 , const _Color aColorStart, const _Color& aLeftColorEdge, const _Color& aRightColorEdge )
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

		_Color lColorLeft = aColorStart + (aLeftColorEdge * sAdjustLeftY);
		_Color lColorRight = aColorStart + (aRightColorEdge * sAdjustRightY);
		_Color lLeftColorInc = aLeftColorEdge / lLeftRowCount;
		_Color lRightColorInc = aRightColorEdge / lRightRowCount;
		
		RasterLines( lLeftStart , aLeftEdge.x  / aLeftEdge.y
				   , lRightStart, aRightEdge.x / aRightEdge.y
				   , lLeftRowCount
				   , lColorLeft, lLeftColorInc
				   , lColorRight, lRightColorInc );
	}
		
	inline void RasterBottomTriangle(const _Point2& aStart, const _Vector2& aLeftEdge, const _Vector2& aRightEdge
									, const _Color aColorStart, const _Color& aLeftColorEdge, const _Color& aRightColorEdge )
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
		_Color lLeftColorInc = -aLeftColorEdge / lLeftRowCount;
		_Color lRightColorInc = -aRightColorEdge / lRightRowCount;
		
		RasterLines( lLeftStart , aLeftEdge.x  / aLeftEdge.y
				   , lRightStart, aRightEdge.x / aRightEdge.y
				   , lLeftRowCount
				   , lColorLeft, lLeftColorInc
				   , lColorRight, lRightColorInc );
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
		sColors[0] = _Color(1,0,0);
		sColors[1] = _Color(0,1,0);
		sColors[2] = _Color(0,0,1);
		RasterTriangle(0, 1, 2);
	}
	
#if 0 // version antigua
	{
		//Ordenar los vertices segun la altura, para partir el triangulo en dos
		if(aV0.y < aV1.y)
		{
			if(aV0.y < aV2.y)
			{
				if(aV1.y < aV2.y)
				{ // 0, 1, 2
					_Vector2 lEdge = aV2 - aV0;
					Real lVal = (aV1.y - aV0.y) / (lEdge.y); // Valor magico
					_Vector2 lSplit = aV0 + (lEdge * lVal); 
					if(aV1.x < aV2.x)
					{ // 1 está a la izquierda
						RasterTopTriangle   (aV0, aV1 - aV0, lSplit - aV0);
						RasterBottomTriangle(aV2, aV1 - aV2, lSplit - aV2);
					}
					else
					{ // 1 está a la derecha
						RasterTopTriangle   (aV0, lSplit - aV0, aV1 - aV0);
						RasterBottomTriangle(aV2, lSplit - aV2, aV1 - aV2);
					}
				}
				else
				{ // 0, 2, 1
					_Vector2 lEdge = aV1 - aV0;
					Real lVal = (aV2.y - aV0.y) / (lEdge.y); // Valor magico
					_Vector2 lSplit = aV0 + (lEdge * lVal); 
					if(aV2.x < aV1.x)
					{ // 2 está a la izquierda
						RasterTopTriangle   (aV0, aV2 - aV0, lSplit - aV0);
						RasterBottomTriangle(aV1, aV2 - aV1, lSplit - aV2);
					}
					else
					{ // 2 está a la derecha
						RasterTopTriangle   (aV0, lSplit - aV0, aV2 - aV0);
						RasterBottomTriangle(aV1, lSplit - aV1, aV2 - aV1);
					}
				}
			}
			else
			{ // 2, 0, 1
				_Vector2 lEdge = aV1 - aV2;
				Real lVal = (aV0.y - aV2.y) / (lEdge.y); // Valor magico
				_Vector2 lSplit = aV2 + (lEdge * lVal); 
				if(aV0.x < aV1.x)
				{ // 0 está a la izquierda
					RasterTopTriangle   (aV2, aV0 - aV2, lSplit - aV2);
					RasterBottomTriangle(aV1, aV0 - aV1, lSplit - aV1);
				}
				else
				{ // 0 está a la derecha
					RasterTopTriangle   (aV2, lSplit - aV2, aV0 - aV2);
					RasterBottomTriangle(aV1, lSplit - aV1, aV0 - aV1);
				}
			}
		}
		else
		{
			if(aV1.y < aV2.y)
			{
				if(aV2.y < aV0.y)
				{ // 1, 2, 0
					_Vector2 lEdge = aV0 - aV1;
					Real lVal = (aV2.y - aV1.y) / (lEdge.y); // Valor magico
					_Vector2 lSplit = aV1 + (lEdge * lVal); 
					if(aV2.x < aV0.x)
					{ // 2 está a la izquierda
						RasterTopTriangle   (aV1, aV2 - aV1, lSplit - aV1);
						RasterBottomTriangle(aV0, aV2 - aV0, lSplit - aV0);
					}
					else
					{ // 2 está a la derecha
						RasterTopTriangle   (aV1, lSplit - aV1, aV2 - aV1);
						RasterBottomTriangle(aV0, lSplit - aV0, aV2 - aV0);
					}
				}
				else
				{ // 1, 0, 2
					_Vector2 lEdge = aV2 - aV1;
					Real lVal = (aV0.y - aV1.y) / (lEdge.y); // Valor magico
					_Vector2 lSplit = aV1 + (lEdge * lVal); 
					if(aV0.x < aV2.x)
					{ // 0 está a la izquierda
						RasterTopTriangle   (aV1, aV0 - aV1, lSplit - aV1);
						RasterBottomTriangle(aV2, aV0 - aV2, lSplit - aV2);
					}
					else
					{ // 0 está a la derecha
						RasterTopTriangle   (aV1, lSplit - aV1, aV0 - aV1);
						RasterBottomTriangle(aV2, lSplit - aV2, aV0 - aV2);
					}
				}
			}
			else
			{ // 2, 1, 0
				_Vector2 lEdge = aV0 - aV2;
				Real lVal = (aV1.y - aV2.y) / (lEdge.y); // Valor magico
				_Vector2 lSplit = aV2 + (lEdge * lVal); 
				if(aV1.x < aV0.x)
				{ // 1 está a la izquierda
					RasterTopTriangle   (aV2, aV1 - aV2, lSplit - aV2);
					RasterBottomTriangle(aV0, aV1 - aV0, lSplit - aV0);
				}
				else
				{ // 1 está a la derecha
					RasterTopTriangle   (aV2, lSplit - aV2, aV1 - aV2);
					RasterBottomTriangle(aV0, lSplit - aV0, aV1 - aV0);
				}
			}
			
		}
	}
#endif
	
	void SetRenderContext(unsigned char* aBuffer, int aWidth, int aHeight)
	{
		Raster::sScreen	= aBuffer;
		Raster::sWidth	= aWidth;
		Raster::sHeight	= aHeight;
	}
	
	void ClearBackBuffer()
	{
		memset(Raster::sScreen, 32, Raster::sWidth * Raster::sHeight * 3);
	}

} }
