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
	static _Vector			sNormals[1024];
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
		struct ScanlineParameters
		{
			struct Edge
			{
			};
			Edge mLeft;
			Edge mRight;
			struct Increments
			{
			} mIncrements;
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

		struct ScanlineParameters
		{
			struct Edge
			{
				_Color mColor;
				_Color mColorGradient;
			};
			Edge mLeft;
			Edge mRight;
			struct Increments
			{
				_Color mColorGradient;
			} mIncrements;
		};
		
	};
	
	struct ColorVertex_Normal
	{
		struct Interpolators 
		{
			// Interpoladores de color
			_Color mLongColorEdge;
			_Color mTopShortColorEdge;
			_Color mBottomShortColorEdge;
			_Color mSplitColor;
			_Vector mLongNormal;
			_Vector mTopNormalEdge;
			_Vector mBottomNormaleEdge;
			_Vector mSplitNormal;
		};
		
		struct ScanlineParameters
		{
			struct Edge
			{
				_Color mColor;
				_Color mColorGradient;
				_Vector mNormal;
				_Vector mNormalGradient;
			};
			Edge mLeft;
			Edge mRight;
			struct Increments
			{
				_Color mColorGradient;
				_Vector mNormalGradient;
			} mIncrements;
		};
	};
	
	template<typename T>
	inline void RasterLines( _Point2& aLeftStart, Real aLeftIncrement
							, _Point2& aRightStart, Real aRightIncrement
							, unsigned aCount
							, typename T::ScanlineParameters::Increments& aIncrements
							, typename T::ScanlineParameters::Edge& aLeftParameters
							, typename T::ScanlineParameters::Edge& aRightParameters ) { /* Do Nothing */  }

	template<>
	inline void RasterLines<ColorVertex>( _Point2& aLeftStart, Real aLeftIncrement
										, _Point2& aRightStart, Real aRightIncrement
										 , unsigned aCount
										 , ColorVertex::ScanlineParameters::Increments& aIncrements
										 , ColorVertex::ScanlineParameters::Edge& aLeft
										 , ColorVertex::ScanlineParameters::Edge& aRight )
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
				_Color lStartColor = ((aRight.mColor - aLeft.mColor) * lAdjustX) + aLeft.mColor;
				
				unsigned lPosition = (lY * Raster::sWidth + unsigned(lLeftScan));
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
			aLeftStart.x+= aLeftIncrement;
			aRightStart.x+= aRightIncrement;
			
			aLeft.mColor+= aLeft.mColorGradient;
			aRight.mColor+= aRight.mColorGradient;
			
			lY++;
		}
	}
	
	template<>
	inline void RasterLines<Draft>( _Point2& aLeftStart, Real aLeftIncrement
								  , _Point2& aRightStart, Real aRightIncrement
								  , unsigned aCount
								  , Draft::ScanlineParameters::Increments& aIncrements
								  , Draft::ScanlineParameters::Edge& aLeft
								  , Draft::ScanlineParameters::Edge& aRight )
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
	inline void AdjustScanlineColors( int i0, int i1, int i2, Real aScale1, Real aScale2
									, typename T::Interpolators& aTable, typename T::ScanlineParameters& aParameters)
	{
		aParameters.mLeft.mColor			= sColors[i0] + (aTable.mTopShortColorEdge * aScale1);
		aParameters.mRight.mColor			= sColors[i0] + (aTable.mLongColorEdge * aScale2);
		aParameters.mLeft.mColorGradient	= aTable.mTopShortColorEdge / aTable.mTopShortEdge.y;
		aParameters.mRight.mColorGradient	= aTable.mLongColorEdge / aTable.mLongEdge.y;
		aParameters.mIncrements.mColorGradient = ( aTable.mSplitColor - sColors[i1] ) / (aTable.mSplit.x - sPositions[i1].x);
	}

	template<>
	inline void AdjustScanlineColors<Draft>( int i0, int i1, int i2, Real aScale1, Real aScale2
										   , Draft::Interpolators& aTable, Draft::ScanlineParameters& aParameters) { /* Do Nothing */ }
	
	template<typename T>
	inline void AdjustScanlineColors( int i0, int i1, int i2, Real aScale
									, typename T::Interpolators& aTable, typename T::ScanlineParameters& aParameters)
	{
		aParameters.mLeft.mColor			= sColors[i1] + (aTable.mBottomShortColorEdge * aScale);
		aParameters.mLeft.mColorGradient	= aTable.mBottomShortColorEdge / aTable.mBottomShortEdge.y;
	}

	template<>
	inline void AdjustScanlineColors<Draft>( int i0, int i1, int i2, Real aScale
										   , Draft::Interpolators& aTable, Draft::ScanlineParameters& aParameters) { /* Do Nothing */ }
	
	template<typename T>
	inline void AdjustScanlineNormals( int i0, int i1, int i2, Real aScale1, Real aScale2
									 , typename T::Interpolators& aTable, typename T::ScanlineParameters& aParameters)
	{
		aParameters.mLeft.mNormal			= sNormals[i0] + (aTable.mTopShortColorEdge * aScale1);
		aParameters.mRight.mNormal			= sNormals[i0] + (aTable.mLongColorEdge * aScale2);
		aParameters.mLeft.mColorGradient	= aTable.mTopNormalEdge / aTable.mTopShortEdge.y;
		aParameters.mRight.mColorGradient	= aTable.mLongNormalEdge / aTable.mLongEdge.y;
		aParameters.mIncrements.mNormalGradient = ( aTable.mSplitNormal - sNormals[i1] ) / (aTable.mSplit.x - sPositions[i1].x);
	}
	
	template<>
	inline void AdjustScanlineNormals<Draft>( int i0, int i1, int i2, Real aScale1, Real aScale2
											, Draft::Interpolators& aTable, Draft::ScanlineParameters& aParameters) { /* Do Nothing */ }
	
	template<>
	inline void AdjustScanlineNormals<ColorVertex>( int i0, int i1, int i2, Real aScale1, Real aScale2
												  , ColorVertex::Interpolators& aTable, ColorVertex::ScanlineParameters& aParameters) { /* Do Nothing */ }
	
	template<typename T>
	inline void AdjustScanlineNormals( int i0, int i1, int i2, Real aScale
									 , typename T::Interpolators& aTable, typename T::ScanlineParameters& aParameters)
	{
		aParameters.mLeft.mNormal			= sColors[i1] + (aTable.mBottomNormalEdge * aScale);
		aParameters.mLeft.mNormalGradient	= aTable.mBottomNormalEdge / aTable.mBottomShortEdge.y;
	}
	
	template<>
	inline void AdjustScanlineNormals<Draft>( int i0, int i1, int i2, Real aScale
									  , Draft::Interpolators& aTable, Draft::ScanlineParameters& aParameters) { /* Do Nothing */ }
	
	template<>
	inline void AdjustScanlineNormals<ColorVertex>( int i0, int i1, int i2, Real aScale
											 , ColorVertex::Interpolators& aTable, ColorVertex::ScanlineParameters& aParameters) { /* Do Nothing */ }
	
	template<typename T>
	inline void _RasterTriangle(int i0, int i1, int i2, typename T::Interpolators& aTable)
	{
		typename T::ScanlineParameters aParameters;
		
		Real lShortGradient	= aTable.mTopShortEdge.x / aTable.mTopShortEdge.y;
		Real lLongGradient	= aTable.mLongEdge.x / aTable.mLongEdge.y;
		
		unsigned lRowCount, lTotalRowCount;
		
		_Vector2 lShortStart	= sPositions[i0];
		Real lShortScale		= AdjustEdge(lShortStart, aTable.mTopShortEdge, lRowCount);
		_Vector2 lLongStart		= sPositions[i0];
		Real lLongScale			= AdjustEdge(lLongStart, aTable.mLongEdge, lTotalRowCount);
		
		AdjustScanlineColors<T>(i0, i1, i2, lShortScale, lLongScale, aTable, aParameters);
		AdjustScanlineNormals<T>(i0, i1, i2, lShortScale, lLongScale, aTable, aParameters);

		if(sPositions[i1].x < sPositions[i2].x)
		{ // 1 está a la izquierda
			RasterLines<T>( lShortStart , lShortGradient, lLongStart, lLongGradient, lRowCount
						  , aParameters.mIncrements, aParameters.mLeft, aParameters.mRight );
		}
		else
		{
			RasterLines<T>( lLongStart, lLongGradient, lShortStart, lShortGradient, lRowCount
						  , aParameters.mIncrements, aParameters.mRight, aParameters.mLeft );
		}

		lShortGradient	= aTable.mBottomShortEdge.x / aTable.mBottomShortEdge.y;
		lShortStart		= sPositions[i1];
		lShortScale		= AdjustEdge(lShortStart, aTable.mBottomShortEdge, lRowCount);

		AdjustScanlineColors<T>(i0, i1, i2, lShortScale, aTable, aParameters);
		AdjustScanlineNormals<T>(i0, i1, i2, lShortScale, aTable, aParameters);
		
		if(sPositions[i1].x < sPositions[i2].x)
		{ // 1 está a la derecha
			RasterLines<T>( lShortStart , lShortGradient, lLongStart, lLongGradient, lRowCount
						  , aParameters.mIncrements, aParameters.mLeft, aParameters.mRight );
		}
		else
		{
			RasterLines<T>( lLongStart, lLongGradient, lShortStart , lShortGradient, lRowCount
						  , aParameters.mIncrements, aParameters.mRight, aParameters.mLeft );
		}
	}
	
	template<typename T>
	inline void InterpolateColors(int i0, int i1, int i2, Real aVal, typename T::Interpolators& aTable)
	{
		aTable.mLongColorEdge			= sColors[i2] - sColors[i0];
		aTable.mTopShortColorEdge		= sColors[i1] - sColors[i0];
		aTable.mBottomShortColorEdge	= sColors[i2] - sColors[i1];
		aTable.mSplitColor				= sColors[i0] + (aTable.mLongColorEdge * aVal);
	}
	
	template<>
	inline void InterpolateColors<Draft>(int i0, int i1, int i2, Real aVal, Draft::Interpolators& aTable) { /* Do Nothing */ }

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
		sColor = aColor * Real(255);
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
		/*for(unsigned i = 0; i < sNumPositions; i+= 3)
		{
			RasterTriangle<Draft>(i+0, i+1, i+2);
			break;
		}*/
		sNumPositions = 0;
	}

} }
