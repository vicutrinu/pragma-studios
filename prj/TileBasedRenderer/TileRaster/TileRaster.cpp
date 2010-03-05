/*
 *  TileRaster.cpp
 *  TileBasedRenderer
 *
 *  Created by Victor on 2/28/10.
 *  Copyright 2010 Apple Inc. All rights reserved.
 *
 */

#include "TileRaster.h"
#include <pragma/math/types.h>
#include <pragma/math/functions.h>

#include <stdio.h>
#include <memory.h>

#define TILE_SIZE 16

namespace pragma { namespace TileRaster
{
	typedef vector2f Position2;
	typedef vector3f Position;
	typedef vector3f Color;
	
	Position sPositions[1024];
	static int sNumPositions = 0;
	Color    sColors[1024];
	
	static unsigned char* sScreen;
	static uint32 sWidth;
	static uint32 sHeight;
	
	template <typename T>
	T Interpolate(float aVal, const T& aSrc, const T& aDest)
	{
		return ((1.f - aVal) * aSrc) + (aVal * aDest);
	}
	
	Position2 TrivialAcceptCorner(const Position2& aTileUpperLeft, const Position2 aEdgeDir)
	{
		if(aEdgeDir.x < 0)
		{
			if(aEdgeDir.y < 0)
			{
				return Position2(aTileUpperLeft.x   , aTileUpperLeft.y + TILE_SIZE);
			}
			else
			{
				return Position2(aTileUpperLeft.x+TILE_SIZE, aTileUpperLeft.y + TILE_SIZE);
			}
		}
		else
		{
			if(aEdgeDir.y < 0)
			{
				return Position2(aTileUpperLeft.x   , aTileUpperLeft.y     );
			}
			else
			{
				return Position2(aTileUpperLeft.x+TILE_SIZE, aTileUpperLeft.y     );
			}

		}
	}
	
	bool PointInsideEdge(const Position2& aPoint, const Position2& aStart, const Position2& aEnd)
	{
		Position2 lA = aPoint - aStart;
		Position2 lB = aEnd - aStart;
		return (lA.x * lB.y - lA.y * lB.x) < 0;
	}

	bool TileInsideTriangle(const Position2& aTilePosition, const Position2& aA, const Position2& aB, const Position2& aC)
    {
		// aA -> aB -> aC tiene que girar en el sentido de las agujas del reloj
		bool lInside;
		lInside =            PointInsideEdge( TrivialAcceptCorner(aTilePosition, aB - aA), aA, aB); 
		lInside = lInside && PointInsideEdge( TrivialAcceptCorner(aTilePosition, aC - aB), aB, aC);
		lInside = lInside && PointInsideEdge( TrivialAcceptCorner(aTilePosition, aA - aC), aC, aA);
		return lInside;
	}
	
	namespace Raster
	{
		struct Simple
		{
			struct Gradients
			{
			};
		};
		
		struct Color
		{
			struct Gradients
			{
				TileRaster::Color mColorX;
				TileRaster::Color mColorY;
			};
		};
	}

	template<typename T>
	void RasterTile(uint32* aData, const typename T::Gradients* aStartValue, const typename T::Gradients* aGradients)
	{
	}
	
	template<>
	void RasterTile<Raster::Simple>(uint32* aData, const Raster::Simple::Gradients* /*aStartValue*/, const Raster::Simple::Gradients* /*aGradients*/)
	{
		const uint32 lColor = 0xFFFFFFFF;
		for(uint32 i = 0; i < TILE_SIZE; ++i)
		{
			for(uint32 j = 0; j < TILE_SIZE; j++)
			{
				aData[j] = lColor;
			}
			aData+= sWidth;
		}
	}

	template<typename T>
	void RasterTile(const uint16* aMask, uint32* aData, const typename T::Gradients* aStartValue, const typename T::Gradients* aGradients)
	{
	}
	
	template<>
	void RasterTile<Raster::Simple>(const uint16* aMask, uint32* aData, const Raster::Simple::Gradients* /*aStartValue*/, const Raster::Simple::Gradients* /*aGradients*/)
	{
		const uint32 lColor = 0xFFFFFFFF;
		for(uint32 i = 0; i < TILE_SIZE; ++i)
		{
			const uint8 lMask = aMask[i];
			aData[0] = lMask & (1 << 15) ? lColor : aData[0];
			aData[1] = lMask & (1 << 14) ? lColor : aData[1];
			aData[2] = lMask & (1 << 13) ? lColor : aData[2];
			aData[3] = lMask & (1 << 12) ? lColor : aData[3];
			aData[4] = lMask & (1 << 11) ? lColor : aData[4];
			aData[5] = lMask & (1 << 10) ? lColor : aData[5];
			aData[6] = lMask & (1 << 9) ? lColor : aData[6];
			aData[7] = lMask & (1 << 8) ? lColor : aData[7];
			aData[8] = lMask & (1 << 7) ? lColor : aData[7];
			aData[9] = lMask & (1 << 6) ? lColor : aData[1];
			aData[10] = lMask & (1 << 5) ? lColor : aData[2];
			aData[11] = lMask & (1 << 4) ? lColor : aData[3];
			aData[12] = lMask & (1 << 3) ? lColor : aData[4];
			aData[13] = lMask & (1 << 2) ? lColor : aData[5];
			aData[14] = lMask & (1 << 1) ? lColor : aData[6];
			aData[15] = lMask & (1 << 0) ? lColor : aData[7];
			aData+= sWidth;
		}
	}
	
	template<typename T>
	void GetGradient( typename T::Gradients* aGradients, float aValX, float aValY, int X0, int X1, int X2, int Y0, int Y1, int Y2, float aLengthX, float aLengthY);
	
	template<>
	inline void GetGradient<Raster::Color>( Raster::Color::Gradients* aGradients, float aValX, float aValY, int X0, int X1, int X2, int Y0, int Y1, int Y2, float aLengthX, float aLengthY )
	{
		aGradients->mColorX = (Interpolate(aValY, sColors[Y0], sColors[Y2]) - sColors[Y1]) / aLengthY;
		aGradients->mColorY = (Interpolate(aValX, sColors[X0], sColors[X2]) - sColors[X1]) / aLengthX;
	}
	
	//--[ Triangle Setup ]--//
	
	inline void GetGradients(int i0, int i1, int i2, Raster::Color::Gradients& aGradients)
	{
		int X0, X1, X2;
		int Y0, Y1, Y2;
		int lTemp;
		
		if(sPositions[i0].y >= sPositions[i1].y) { Y0 = i1; Y1 = i0; } else { Y0 = i0; Y1 = i1; }
		if(sPositions[Y1].y >= sPositions[i2].y) { Y2 = Y1; Y1 = i2; } else { Y2 = i2; }
		if(sPositions[Y0].y >= sPositions[Y1].y) { lTemp = Y0; Y0 = Y1; Y1 = lTemp; }
		
		if(sPositions[i0].x >= sPositions[i1].x) { X0 = i1; X1 = i0; } else { X0 = i0; X1 = i1; }
		if(sPositions[X1].x >= sPositions[i2].x) { X2 = X1; X1 = i2; } else { X2 = i2; }
		if(sPositions[X0].x >= sPositions[X1].x) { lTemp = X0; X0 = X1; X1 = lTemp; }
		
		float lValX = (sPositions[X1].x - sPositions[X0].x) / (sPositions[X2].x - sPositions[X0].x);
		float lValY = (sPositions[Y1].y - sPositions[Y0].y) / (sPositions[Y2].y - sPositions[Y0].y);
		
		float lSplitX = ((1-lValX) * sPositions[X0].y) + (lValX * sPositions[X2].y);
		float lSplitY = ((1-lValY) * sPositions[Y0].x) + (lValY * sPositions[Y2].x);

		GetGradient<Raster::Color>( &aGradients, lValX, lValY
								  , X0, X1, X2
								  , Y0, Y1, Y2
								  , lSplitX - sPositions[X1].y
								  , lSplitY - sPositions[Y1].x );
	}
	
	uint32 ConvertColor(const Color& aColor)
	{
		return ((int(aColor.x * 255)<<0)&255) +
			   ((int(aColor.y * 255)<<8)&(255<<8)) + 
			   ((int(aColor.z * 255)<<16)&(255<<16));
	}
	
	void RasterTriangle(int i0, int i1, int i2)
	{
		Raster::Color::Gradients lGradients;
		GetGradients(i0, i1, i2, lGradients);
		uint32* lPtr = (uint32*)sScreen;
		Color lStart(1,0,0);
		lStart = lStart - (lGradients.mColorX * 20.f);
		lStart = lStart - (lGradients.mColorY * 20.f);
		/*for(int i = 0; i < sHeight; ++i)
		{
			Color lValue = lStart; lStart+= lGradients.mColorY;
			for(int j = 0; j < sWidth; ++j)
			{
				*lPtr++ = ConvertColor(lValue);
				lValue+= lGradients.mColorX;
			}
		}*/
		
		Position2 lA(sPositions[i0].x, sPositions[i0].y);
		Position2 lB(sPositions[i1].x, sPositions[i1].y);
		Position2 lC(sPositions[i2].x, sPositions[i2].y);
		Position2 lTemp;
		if(PointInsideEdge(lC, lA, lB))
		{
			
		}
		else
		{
			lTemp = lA;
			lA = lB;
			lB = lTemp;
		}

		for(int i = 0; i < sHeight; i+= TILE_SIZE)
		{
			lPtr = (uint32*)sScreen + (sWidth*i);
			for(int j = 0; j < sWidth; j+= TILE_SIZE)
			{
				if(TileInsideTriangle(Position2(j, i), lA, lB, lC))
				{
					RasterTile<Raster::Simple>(lPtr, 0, 0);
				}
				lPtr+= TILE_SIZE;
			}
		}
	}
	
	void SetRenderContext(unsigned char* aBuffer, int aWidth, int aHeight)
	{
		sScreen	= aBuffer;
		sWidth	= aWidth;
		sHeight	= aHeight;
	}
	
	void ClearBackBuffer()
	{
		//memset(sScreen, 32, sWidth * sHeight * 4);
		sNumPositions = 0;
	}
	
	void Render()
	{
		//sPositions[0] = Position( 20, 20, 0);
		sColors[0]	  = Color(1,0,0);
		//sPositions[1] = Position(512-20, 20, 0);
		sColors[1]    = Color(0,1,0);
		//sPositions[2] = Position(20, 512-20, 0);
		sColors[2]    = Color(0,0,1);
		for(unsigned i = 0; i < sNumPositions; i+= 3)
			RasterTriangle(i+0,i+1,i+2);
		
	}
	
	void AddVertex(const vector2f& aPosition)
	{
		sPositions[sNumPositions] = Position(aPosition.x, aPosition.y, 0);
		sNumPositions++;
	}
		
} }

