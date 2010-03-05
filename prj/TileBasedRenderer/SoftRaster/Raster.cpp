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
	static UV               sUV;
	static _Point2			sPositions[1024];
	static _Color			sColors[1024];
	static _Vector			sNormals[1024];
	static UV				sUVs[1024];
	static unsigned			sNumPositions = 0;
	static EMode			sRasterMode = eSimple;	
} }

#include "functions.h"		// Generic functions
#include "internal_types.h"
#include "Color.h"
#include "Texture.h"
#include "Position.h"
#include "TextureModulate.h"
#include "Gouraud.h"

namespace pragma { namespace Raster
{
	
	// General RasterTriangle function
	template<typename RASTERTYPE>
	inline void _RasterTriangle(int i0, int i1, int i2, typename RASTERTYPE::InterpolatorType::Interpolators& aTable)
	{
		typename RASTERTYPE::InterpolatorType::ScanlineParameters aParameters;
		
		Real lShortGradient	= aTable.mTopEdge.x / aTable.mTopEdge.y;
		Real lLongGradient	= aTable.mLongEdge.x / aTable.mLongEdge.y;
		
		unsigned lRowCount, lTotalRowCount;
		
		_Vector2 lShortStart	= sPositions[i0];
		Real lShortScale		= AdjustEdge(lShortStart, aTable.mTopEdge, lRowCount);
		_Vector2 lLongStart		= sPositions[i0];
		Real lLongScale			= AdjustEdge(lLongStart, aTable.mLongEdge, lTotalRowCount);
		
		AdjustScanlineColors<typename RASTERTYPE::InterpolatorType>	(i0, i1, i2, lShortScale, lLongScale, aTable, aParameters);
		AdjustScanlineNormals<typename RASTERTYPE::InterpolatorType>(i0, i1, i2, lShortScale, lLongScale, aTable, aParameters);
		AdjustScanlineUVs<typename RASTERTYPE::InterpolatorType>	(i0, i1, i2, lShortScale, lLongScale, aTable, aParameters);
		
		unsigned lY = lShortStart.y;

		if(sPositions[i1].x < sPositions[i2].x)
		{ // 1 está a la izquierda
			RasterLines<typename RASTERTYPE::InterpolatorType,RASTERTYPE>( lShortStart.x, lLongStart.x, lY, lShortGradient, lLongGradient, lRowCount
																		 , aParameters.mIncrements, aParameters.mLeft, aParameters.mRight );
		}
		else
		{
			RasterLines<typename RASTERTYPE::InterpolatorType,RASTERTYPE>( lLongStart.x, lShortStart.x, lY, lLongGradient, lShortGradient, lRowCount
																		 , aParameters.mIncrements, aParameters.mRight, aParameters.mLeft );
		}

		lShortGradient	= aTable.mBottomEdge.x / aTable.mBottomEdge.y;
		lShortStart		= sPositions[i1];
		lShortScale		= AdjustEdge(lShortStart, aTable.mBottomEdge, lRowCount);

		AdjustScanlineColors<typename RASTERTYPE::InterpolatorType>	(i0, i1, i2, lShortScale, aTable, aParameters);
		AdjustScanlineNormals<typename RASTERTYPE::InterpolatorType>(i0, i1, i2, lShortScale, aTable, aParameters);
		AdjustScanlineUVs<typename RASTERTYPE::InterpolatorType>	(i0, i1, i2, lShortScale, aTable, aParameters);
		
		if(sPositions[i1].x < sPositions[i2].x)
		{ // 1 está a la derecha
			RasterLines<typename RASTERTYPE::InterpolatorType,RASTERTYPE>( lShortStart.x, lLongStart.x, lY, lShortGradient, lLongGradient, lRowCount
																		 , aParameters.mIncrements, aParameters.mLeft, aParameters.mRight );
		}
		else
		{
			RasterLines<typename RASTERTYPE::InterpolatorType,RASTERTYPE>( lLongStart.x, lShortStart.x, lY, lLongGradient, lShortGradient, lRowCount
																		 , aParameters.mIncrements, aParameters.mRight, aParameters.mLeft );
		}
	}

	template<typename RASTERTYPE>
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
		
		typename RASTERTYPE::InterpolatorType::Interpolators lTable;
		
		Real lVal = (sPositions[i1].y - sPositions[i0].y) / (sPositions[i2].y - sPositions[i0].y); // Valor magico que nos lleva a la "mitad" del triangulo

		lTable.mLongEdge			= sPositions[i2] - sPositions[i0];
		lTable.mTopEdge				= sPositions[i1] - sPositions[i0];
		lTable.mBottomEdge			= sPositions[i2] - sPositions[i1];
		lTable.mSplit				= sPositions[i0] + (lTable.mLongEdge * lVal); // lTable.mSplit = (1-lVal) * sPositions[i0] + lVal * sPositions[i2];

		InitColors<typename RASTERTYPE::InterpolatorType>(i0, i1, i2, lVal, lTable );
		InitUVs<typename RASTERTYPE::InterpolatorType>(i0,i1,i2, lVal, lTable);
		InitNormals<typename RASTERTYPE::InterpolatorType>(i0, i1, i2, lVal, lTable);

		_RasterTriangle<RASTERTYPE>(i0, i1, i2, lTable);
	}
	
	void AddVertex(const _Point2& aPosition)
	{
		sPositions[sNumPositions] = aPosition;
		sColors[sNumPositions] = sColor;
		sUVs[sNumPositions] = sUV;
		sNumPositions++;
	}
	
	void VertexColor(const _Color& aColor)
	{
		sColor = aColor * Real(255);
	}
	
	void VertexUV(const UV& aUV)
	{
		sUV = aUV;
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
	
	void SetRasterMode(EMode aMode)
	{
		sRasterMode = aMode;
	}
	
	void Render()
	{
		switch(sRasterMode)
		{
			case eVertexColor:
				for(unsigned i = 0; i < sNumPositions; i+= 3)
				{
					RasterTriangle<VertexColorRaster>(i+0, i+1, i+2);
				}
				break;
			case eTexture:
				for(unsigned i = 0; i < sNumPositions; i+= 3)
				{
					RasterTriangle<TextureRaster>(i+0, i+1, i+2);
				}
				break;
			case eSimple:
				for(unsigned i = 0; i < sNumPositions; i+= 3)
				{
					RasterTriangle<SimpleRaster>(i+0, i+1, i+2);
				}
				break;
			case eTextureModulate:
				for(unsigned i = 0; i < sNumPositions; i+= 3)
				{
					RasterTriangle<TextureModulateRaster>(i+0, i+1, i+2);
				}
				break;
			case eGouraud:
				for(unsigned i = 0; i < sNumPositions; i+= 3)
				{
					RasterTriangle<GouraudRaster>(i+0, i+1, i+2);
				}
				break;
		}
		sNumPositions = 0;
	}

} }
