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
#include <pragma/math/math.h>
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
	
	static unsigned char* sScreen = 0;
	static uint32 sWidth;
	static uint32 sHeight;
	static uint32 sTilesWidth;
	static uint32 sTilesHeight;
	
	Position2* sTileCorners = 0;
	char*		sTileStats = 0;
	
	template <typename T>
	T Interpolate(float aVal, const T& aSrc, const T& aDest)
	{
		return ((1.f - aVal) * aSrc) + (aVal * aDest);
	}
	
	uint32 ConvertColor(const Color& aColor)
	{
		return	((int(aColor.x * 255)<<0)&255) +
				((int(aColor.y * 255)<<8)&(255<<8)) + 
				((int(aColor.z * 255)<<16)&(255<<16));
	}
	
	Position2 TrivialAcceptCorner(uint32 aTile, const Position2 aEdgeDir)
	{
		if(aEdgeDir.x < 0)
		{
			if(aEdgeDir.y < 0)
				return sTileCorners[aTile + sTilesWidth+1 + 0];
			else
				return sTileCorners[aTile + sTilesWidth+1 + 1];
		}
		else
		{
			if(aEdgeDir.y < 0)
				return sTileCorners[aTile + 0];
			else
				return sTileCorners[aTile + 1];
		}
	}
	
	Position2 TrivialRejectCorner(uint32 aTile, const Position2 aEdgeDir)
	{
		if(aEdgeDir.x < 0)
		{
			if(aEdgeDir.y < 0)
				return sTileCorners[aTile + 1];
			else
				return sTileCorners[aTile];
		}
		else
		{
			if(aEdgeDir.y < 0)
				return sTileCorners[aTile + sTilesWidth+1 + 1];
			else
				return sTileCorners[aTile + sTilesWidth+1 + 0];
		}
	}
	
	bool PointInsideEdge(const Position2& aPoint, const Position2& aStart, const Position2& aEnd)
	{
		Position2 lA = aPoint - aStart;
		Position2 lB = aEnd - aStart;
		return (lA.x * lB.y - lA.y * lB.x) < 0;
	}

	bool TileInsideTriangle(uint32 aTile, const Position2& aA, const Position2& aB, const Position2& aC)
    {
		// aA -> aB -> aC tiene que girar en el sentido de las agujas del reloj
		bool lInside;
		lInside =            PointInsideEdge( TrivialAcceptCorner(aTile, aB - aA), aA, aB); 
		lInside = lInside && PointInsideEdge( TrivialAcceptCorner(aTile, aC - aB), aB, aC);
		lInside = lInside && PointInsideEdge( TrivialAcceptCorner(aTile, aA - aC), aC, aA);
		return lInside;
	}
	
	bool PointInsideTriangle(const Position2& aPoint, const Position2& aA, const Position2& aB, const Position2& aC)
    {
		// aA -> aB -> aC tiene que girar en el sentido de las agujas del reloj
		bool lInside;
		lInside =            PointInsideEdge( aPoint, aA, aB); 
		lInside = lInside && PointInsideEdge( aPoint, aB, aC);
		lInside = lInside && PointInsideEdge( aPoint, aC, aA);
		return lInside;
	}
	
	bool TileOutsideTriangle(uint32 aTile, const Position2& aA, const Position2& aB, const Position2& aC)
    {
		// aA -> aB -> aC tiene que girar en el sentido de las agujas del reloj
		bool lOutside;
		lOutside =	( !PointInsideEdge( TrivialRejectCorner(aTile, aB - aA), aA, aB)) || 
					( !PointInsideEdge( TrivialRejectCorner(aTile, aC - aB), aB, aC)) ||
					( !PointInsideEdge( TrivialRejectCorner(aTile, aA - aC), aC, aA));
		return lOutside;
	}
	
	namespace Raster
	{
		struct Simple
		{
			struct Start
			{
			};
			struct Gradients
			{
			};
		};
		
		struct Color
		{
			struct Start
			{
				TileRaster::Color mColor;
			};
			struct Gradients
			{
				TileRaster::Color mColorX;
				TileRaster::Color mColorY;
			};
		};
	}

	template<typename T>
	void RasterTile(uint32* aData, const typename T::Start* aStartValue, const typename T::Gradients* aGradients)
	{
	}
	
	template<typename T>
	void RasterTile(const uint8* aMask, uint32* aData, const typename T::Start* aStartValue, const typename T::Gradients* aGradients)
	{
	}

	template<>
	void RasterTile<Raster::Simple>(uint32* aData, const Raster::Simple::Start* /*aStartValue*/, const Raster::Simple::Gradients* /*aGradients*/)
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
	
	template<>
	void RasterTile<Raster::Simple>(const uint8* aMask, uint32* aData, const Raster::Simple::Start* /*aStartValue*/, const Raster::Simple::Gradients* /*aGradients*/)
	{
		const uint32 lColor = 0xFFFFFFFF;
		const uint8* lMask = aMask;
		for(uint32 i = 0; i < TILE_SIZE; ++i)
		{
			if(lMask[ 0] != 0) aData[0] = lColor;
			if(lMask[ 1] != 0) aData[1] = lColor;
			if(lMask[ 2] != 0) aData[2] = lColor;
			if(lMask[ 3] != 0) aData[3] = lColor;
			if(lMask[ 4] != 0) aData[4] = lColor;
			if(lMask[ 5] != 0) aData[5] = lColor;
			if(lMask[ 6] != 0) aData[6] = lColor;
			if(lMask[ 7] != 0) aData[7] = lColor;
			if(lMask[ 8] != 0) aData[8] = lColor;
			if(lMask[ 9] != 0) aData[9] = lColor;
			if(lMask[10] != 0) aData[10] = lColor;
			if(lMask[11] != 0) aData[11] = lColor;
			if(lMask[12] != 0) aData[12] = lColor;
			if(lMask[13] != 0) aData[13] = lColor;
			if(lMask[14] != 0) aData[14] = lColor;
			if(lMask[15] != 0) aData[15] = lColor;
			lMask+= 16;
			aData+= sWidth;
		}
	}
	
	template<>
	void RasterTile<Raster::Color>(uint32* aData, const Raster::Color::Start* aStartValue, const Raster::Color::Gradients* aGradients)
	{
		for(uint32 i = 0; i < TILE_SIZE; ++i)
		{
			TileRaster::Color lColor = aStartValue->mColor + (aGradients->mColorY * (float)i);
			for(uint32 j = 0; j < TILE_SIZE; j++)
			{
				aData[j] = ConvertColor(lColor);
				lColor = lColor + aGradients->mColorX;
			}
			aData+= sWidth;
		}
	}
	
	template<>
	void RasterTile<Raster::Color>(const uint8* aMask, uint32* aData, const Raster::Color::Start* aStartValue, const Raster::Color::Gradients* aGradients)
	{
		const uint8* lMask = aMask;
		for(uint32 i = 0; i < TILE_SIZE; ++i)
		{
			TileRaster::Color lColor = aStartValue->mColor + (aGradients->mColorY * (float)i);
			for(uint32 j = 0; j < TILE_SIZE; j++)
			{
				
					if(*lMask++ != 0)
						aData[j] = ConvertColor(lColor);
					else
					{
						if(aData[j] == 0x20202020)
						{ // Debug
							aData[j] = 0xFFFFFFFF;
						}
					}
				lColor = lColor + aGradients->mColorX;
			}
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
	
	void GenerateMask(uint8* aMask, const Position2& aTilePosition, const Position2& a0, const Position2& a1, const Position2& a2)
	{
		uint8* lPtr = aMask;
		for(uint32 i = 0; i < TILE_SIZE; i++)
		{
			for(uint32 j = 0; j < TILE_SIZE; j++)
			{
				*lPtr++ = PointInsideTriangle(Position2(aTilePosition.x+j, aTilePosition.y + i), a0, a1, a2);
			}
		}
	}
	
	
	
	void RasterTriangle(int i0, int i1, int i2)
	{
		Raster::Color::Gradients lGradients;
		GetGradients(i0, i1, i2, lGradients);
		uint32* lPtr = (uint32*)sScreen;
		Color lStart = sColors[i0];//Color(1,0,0);
		lStart = lStart - (lGradients.mColorX * sPositions[i0].x);
		lStart = lStart - (lGradients.mColorY * sPositions[i0].y);
		/*for(int i = 0; i < sHeight; ++i)
		{
			Color lValue = lStart; lStart+= lGradients.mColorY;
			for(int j = 0; j < sWidth; ++j)
			{
				*lPtr++ = ConvertColor(lValue);
				lValue+= lGradients.mColorX;
			}
		}
		return;*/
		
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
		
		Raster::Color::Start lPollas;
		
		for(uint32 i = 0; i < sTilesHeight ; i++)
		{
			lPollas.mColor = lStart + (lGradients.mColorY * float(TILE_SIZE*i));
			lPtr = (uint32*)sScreen + ((i*TILE_SIZE) * sWidth);
			for(uint32 j = 0; j < sTilesWidth; j++)
			{
				if(!TileOutsideTriangle(i * (sTilesWidth + 1) + j, lA, lB, lC))
				{
					if(TileInsideTriangle(i * (sTilesWidth + 1) + j , lA, lB, lC))
						RasterTile<Raster::Color>(lPtr, &lPollas, &lGradients);
					else
					{
						// Partially inside
						uint8 lMask[TILE_SIZE * TILE_SIZE];
						GenerateMask(lMask, Position2(j*TILE_SIZE, i * TILE_SIZE), lA, lB, lC);
						RasterTile<Raster::Color>(lMask, lPtr, &lPollas, &lGradients);
					}
				}

				lPollas.mColor = lPollas.mColor + (lGradients.mColorX * float(TILE_SIZE));
				lPtr+= TILE_SIZE;
			}
		}
	}
	
	void SetRenderContext(unsigned char* aBuffer, int aWidth, int aHeight)
	{
		sScreen	= aBuffer;
		sWidth	= aWidth;
		sHeight	= aHeight;
		sTilesWidth = aWidth / TILE_SIZE;
		sTilesHeight = aHeight / TILE_SIZE;
		if(sTileCorners)
			delete[] sTileCorners;
		sTileCorners = new Position2[(sTilesWidth + 1) * (sTilesHeight + 1)];
		Position2* lPtr = sTileCorners;
		for(uint32 i = 0; i < sTilesHeight+1; ++i)
		{
			for(uint32 j = 0; j < sTilesWidth+1; ++j)
			{
				lPtr->x = TILE_SIZE * j;
				lPtr->y = TILE_SIZE * i;
				lPtr++;
			}
		}

		if(sTileStats)
			delete[] sTileStats;
		sTileStats = new char[sTilesWidth * sTilesHeight];
	}
	
	void ClearBackBuffer()
	{
		//memset(sScreen, 32, sWidth * sHeight * 4);
		sNumPositions = 0;
		memset(sTileStats, 0, sTilesWidth * sTilesHeight);
	}
	
	void Render()
	{
		/*sPositions[1] = Position( 256+50, 50, 0);
		sColors[0]	  = Color(1,0,0);
		sPositions[2] = Position( 100, 300, 0);
		sColors[1]    = Color(0,1,0);
		sPositions[0] = Position( 10, 10, 0);
		sColors[2]    = Color(0,0,1);
		RasterTriangle(0,1,2);*/
		for(unsigned i = 0; i < sNumPositions; i+= 3)
			RasterTriangle(i+0,i+1,i+2);
	}
	
	void AddVertex(const vector2f& aPosition)
	{
		sPositions[sNumPositions] = Position(aPosition.x, aPosition.y, 0);
		sNumPositions++;
	}
	
	void AddColor(const vector3f& aColor)
	{
		sColors[sNumPositions] = Color(aColor.x, aColor.y, aColor.z);
	}
		
} }

