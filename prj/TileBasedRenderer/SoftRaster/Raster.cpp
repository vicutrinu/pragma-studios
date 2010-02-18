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

#include "internal_types.h"

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

	template<typename T>
	inline void InterpolateColors(int i0, int i1, int i2, Real aVal, typename T::Interpolators& aTable)
	{
		aTable.mLongColorEdge			= sColors[i2] - sColors[i0];
		aTable.mTopShortColorEdge		= sColors[i1] - sColors[i0];
		aTable.mBottomShortColorEdge	= sColors[i2] - sColors[i1];
		aTable.mSplitColor				= sColors[i0] + (aTable.mLongColorEdge * aVal);
	}
	
	template<typename T>
	inline void InterpolateUVs(int i0, int i1, int i2, Real aVal, typename T::Interpolators& aTable)
	{
		aTable.mLongUVEdge		= sUVs[i2] - sUVs[i0];
		aTable.mTopUVEdge		= sUVs[i1] - sUVs[i0];
		aTable.mBottomUVEdge	= sUVs[i2] - sUVs[i1];
		aTable.mSplitUV			= sUVs[i0] + (aTable.mLongUVEdge * aVal);
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

	template<typename T>
	inline void AdjustScanlineColors( int i0, int i1, int i2, Real aScale
									, typename T::Interpolators& aTable, typename T::ScanlineParameters& aParameters)
	{
		aParameters.mLeft.mColor			= sColors[i1] + (aTable.mBottomShortColorEdge * aScale);
		aParameters.mLeft.mColorGradient	= aTable.mBottomShortColorEdge / aTable.mBottomShortEdge.y;
	}
	
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

	template<typename T>
	inline void AdjustScanlineNormals( int i0, int i1, int i2, Real aScale
									 , typename T::Interpolators& aTable, typename T::ScanlineParameters& aParameters)
	{
		aParameters.mLeft.mNormal			= sColors[i1] + (aTable.mBottomNormalEdge * aScale);
		aParameters.mLeft.mNormalGradient	= aTable.mBottomNormalEdge / aTable.mBottomShortEdge.y;
	}

	template<typename T>
	inline void AdjustScanlineUVs( int i0, int i1, int i2, Real aScale1, Real aScale2
									  , typename T::Interpolators& aTable, typename T::ScanlineParameters& aParameters)
	{
		aParameters.mLeft.mUV				= sUVs[i0] + (aTable.mTopUVEdge * aScale1);
		aParameters.mRight.mUV				= sUVs[i0] + (aTable.mLongUVEdge * aScale2);
		aParameters.mLeft.mUVGradient		= aTable.mTopUVEdge / aTable.mTopShortEdge.y;
		aParameters.mRight.mUVGradient		= aTable.mLongUVEdge / aTable.mLongEdge.y;
		aParameters.mIncrements.mUVGradient = ( aTable.mSplitUV - sUVs[i1] ) / (aTable.mSplit.x - sPositions[i1].x);
	}
	
	template<typename T>
	inline void AdjustScanlineUVs( int i0, int i1, int i2, Real aScale
									  , typename T::Interpolators& aTable, typename T::ScanlineParameters& aParameters)
	{
		aParameters.mLeft.mUV			= sUVs[i1] + (aTable.mBottomUVEdge * aScale);
		aParameters.mLeft.mUVGradient	= aTable.mBottomUVEdge / aTable.mBottomShortEdge.y;
	}

#include "functions.h"		// Generic functions
// Vertex formats
#include "Texture.h"
#include "ColorVertex.h"	
#include "Draft.h"

	
	// General RasterTriangle function
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
		AdjustScanlineUVs<T>(i0, i1, i2, lShortScale, lLongScale, aTable, aParameters);
		
		unsigned lY = lShortStart.y;

		if(sPositions[i1].x < sPositions[i2].x)
		{ // 1 está a la izquierda
			RasterLines<T>( lShortStart.x, lLongStart.x, lY, lShortGradient, lLongGradient, lRowCount
						  , aParameters.mIncrements, aParameters.mLeft, aParameters.mRight );
		}
		else
		{
			RasterLines<T>( lLongStart.x, lShortStart.x, lY, lLongGradient, lShortGradient, lRowCount
						  , aParameters.mIncrements, aParameters.mRight, aParameters.mLeft );
		}

		lShortGradient	= aTable.mBottomShortEdge.x / aTable.mBottomShortEdge.y;
		lShortStart		= sPositions[i1];
		lShortScale		= AdjustEdge(lShortStart, aTable.mBottomShortEdge, lRowCount);

		AdjustScanlineColors<T>(i0, i1, i2, lShortScale, aTable, aParameters);
		AdjustScanlineNormals<T>(i0, i1, i2, lShortScale, aTable, aParameters);
		AdjustScanlineUVs<T>(i0, i1, i2, lShortScale, aTable, aParameters);
		
		if(sPositions[i1].x < sPositions[i2].x)
		{ // 1 está a la derecha
			RasterLines<T>( lShortStart.x, lLongStart.x, lY, lShortGradient, lLongGradient, lRowCount
						  , aParameters.mIncrements, aParameters.mLeft, aParameters.mRight );
		}
		else
		{
			RasterLines<T>( lLongStart.x, lShortStart.x, lY, lLongGradient, lShortGradient, lRowCount
						  , aParameters.mIncrements, aParameters.mRight, aParameters.mLeft );
		}
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
		InterpolateUVs<T>(i0,i1,i2, lVal, lTable);
		
		_RasterTriangle<T>(i0, i1, i2, lTable);
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
	
	void Render()
	{
		for(unsigned i = 0; i < sNumPositions; i+= 3)
		{
			RasterTriangle<Texture>(i+0, i+1, i+2);
		}
		/*for(unsigned i = 0; i < sNumPositions; i+= 3)
		{
			RasterTriangle<Draft>(i+0, i+1, i+2);
			break;
		}*/
		sNumPositions = 0;
	}

} }
