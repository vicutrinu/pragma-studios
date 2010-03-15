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
	// Types
	typedef float	 Real;
	typedef vector2f Position2;
	typedef vector3f Position;
	typedef vector3f Color;
	typedef vector3f UV;

	// Internal Engine Data
	//	Geometry
	Position				sPositions[1024];
	static int				sNumPositions = 0;
	static Color			sColors[1024];
	static UV				sUVs[1024];
	//	Screen & other internal buffers
	static unsigned char*	sScreen = 0;
	static uint32			sWidth;
	static uint32			sHeight;
	static uint32			sTilesWidth;
	static uint32			sTilesHeight;
	Position2*				sTileCorners = 0;
	char*					sTileStats = 0;

	/**
	 *	Interpolacion lineal entre aSrc y aDest
	 */
	template <typename T>
	static inline T Interpolate(Real aVal, const T& aSrc, const T& aDest)
	{
		return ((Real(1) - aVal) * aSrc) + (aVal * aDest);
	}

	/**
	 *	Convierte un color del tipo Color a un entero de 32 bits
	 */
	static inline uint32 ConvertColor(const Color& aColor)
	{
		return	((int(clamp<Real>(aColor.x, 0, 1) * 255) <<  0) & (255 <<  0)) |
				((int(clamp<Real>(aColor.y, 0, 1) * 255) <<  8) & (255 <<  8)) | 
				((int(clamp<Real>(aColor.z, 0, 1) * 255) << 16) & (255 << 16));
	}
	
	static inline Position2 TrivialAcceptCorner(uint32 aTile, const Position2& aEdgeDir)
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
	
	static inline Position2 TrivialRejectCorner(uint32 aTile, const Position2& aEdgeDir)
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

	/**
	 *	Determina la posicion de un punto respecto a una linea
	 *	Dada la direccion de avance del segmento, de aStart hacia aEnd, se dice que el punto aPoing está "dentro" 
	 *	si que en el lado izquierdo, y fuera en caso contrario.
	 *	Si el punto forma parte de la linea entonces tambien se considera que está dentro
	 */
	static inline bool PointInsideEdge(const Position2& aPoint, const Position2& aStart, const Position2& aEnd)
	{
		Position2 lA = aPoint - aStart;
		Position2 lB = aEnd - aStart;
		return (lA.x * lB.y - lA.y * lB.x) < 0;
	}

	/**
	 *	Determina si un Tile está completamente dentro de un triangulo
	 */
	static inline bool TileInsideTriangle(uint32 aTile, const Position2& aA, const Position2& aB, const Position2& aC)
    {
		// aA -> aB -> aC tiene que girar en el sentido de las agujas del reloj
		bool lInside;
		lInside =            PointInsideEdge( TrivialAcceptCorner(aTile, aB - aA), aA, aB); 
		lInside = lInside && PointInsideEdge( TrivialAcceptCorner(aTile, aC - aB), aB, aC);
		lInside = lInside && PointInsideEdge( TrivialAcceptCorner(aTile, aA - aC), aC, aA);
		return lInside;
	}
	
	/**
	 *	Determina si un punto está completamente dentro de un triangulo
	 */
	static inline bool PointInsideTriangle(const Position2& aPoint, const Position2& aA, const Position2& aB, const Position2& aC)
    {
		// aA -> aB -> aC tiene que girar en el sentido de las agujas del reloj
		bool lInside;
		lInside =            PointInsideEdge( aPoint, aA, aB); 
		lInside = lInside && PointInsideEdge( aPoint, aB, aC);
		lInside = lInside && PointInsideEdge( aPoint, aC, aA);
		return lInside;
	}

	/**
	 *	Determina si un Tile está fuera de un triangulo
	 */
	static inline bool TileOutsideTriangle(uint32 aTile, const Position2& aA, const Position2& aB, const Position2& aC)
    {
		// aA -> aB -> aC tiene que girar en el sentido de las agujas del reloj
		bool lOutside;
		lOutside =	( !PointInsideEdge( TrivialRejectCorner(aTile, aB - aA), aA, aB)) || 
					( !PointInsideEdge( TrivialRejectCorner(aTile, aC - aB), aB, aC)) ||
					( !PointInsideEdge( TrivialRejectCorner(aTile, aA - aC), aC, aA));
		return lOutside;
	}

#define RASTER(aName)	struct aName : public Base 
#define COMPONENTS		struct Start
#define GRADIENTS		struct Gradients
#define COLOR_COMPONENT TileRaster::Color	mColor;
#define COLOR_GRADIENTS TileRaster::Color	mColorX; TileRaster::Color mColorY;
#define UV_COMPONENT	TileRaster::UV		mUV;
#define UV_GRADIENTS	TileRaster::UV		mUVX; TileRaster::UV mUVY;
#define COLOR_PRESENT	typedef Present Color
#define UV_PRESENT		typedef Present UV
	
	namespace Raster
	{		
		struct Present { };
		struct NotPresent { };
		
		struct Base
		{
			typedef NotPresent Color;
			typedef NotPresent UV;
		};

		/**
		 *	Definiciones de Rasters
		 *	Cada nuevo Raster debe heredar de la clase 'Base'. Por cada Iterador que utilice deberá
		 *	declarar los componentes y gradientes
		 *	Con esto indicaremos a los templates que deben incluir el codigo para iterar los valores
		 *	de color de vertice
		 */
		
		// Pinta un color solido configurado en el motor. No necesita iterar nada
		RASTER(Simple)
		{
			COMPONENTS	{ };
			GRADIENTS	{ };
		};

		// Pinta iterando los vertex colors
		RASTER(VertexColor)
		{
			COLOR_PRESENT;
			COMPONENTS	{ COLOR_COMPONENT };
			GRADIENTS	{ COLOR_GRADIENTS };
		};

		// Solo pinta la textura
		RASTER(Texture)
		{
			typedef Present UV;
			COMPONENTS	{ UV_COMPONENT };
			GRADIENTS	{ UV_GRADIENTS };
		};

		// Multiplica el ColorVertex por la textura
		RASTER(TextureModulate)
		{
			COLOR_PRESENT;
			UV_PRESENT;
			COMPONENTS	{ COLOR_COMPONENT UV_COMPONENT };
			GRADIENTS	{ COLOR_GRADIENTS UV_GRADIENTS };
		};
		
	}

	template<typename T>
	void RasterTile							( uint32* aData, const typename T::Start* aStartValue, const typename T::Gradients* aGradients ) { }
	
	template<typename T>
	void RasterTile							( const uint8* aMask, uint32* aData, const typename T::Start* aStartValue, const typename T::Gradients* aGradients ) { }
	
	template<typename COMPONENT_PRESENT>
	class Gradient
	{
	public:
		template<typename T>
		static inline void GetColor			( typename T::Gradients* aGradients, float aValX, float aValY, int X0, int X1, int X2, int Y0, int Y1, int Y2, float aLengthX, float aLengthY) { }
		
		template<typename T>
		static inline void GetUV			( typename T::Gradients* aGradients, float aValX, float aValY, int X0, int X1, int X2, int Y0, int Y1, int Y2, float aLengthX, float aLengthY) { }
	};
	
	template<typename COMPONENT_PRESENT>
	class Start
	{
	public:
		template<typename T>
		static inline void GetColor			( int i0, typename T::Gradients& aGradients, typename T::Start& aStarts) { }
		
		template<typename T>
		static inline void GetUV			( int i0, typename T::Gradients& aGradients, typename T::Start& aStarts) { }
	};

	template<typename COMPONENT_PRESENT>
	class TileSetup
	{
	public:
		template<typename T>
		static inline void MoveColor_Right	( int aCount, typename T::Start& aPos, const typename T::Gradients& aGradients ) { }
		template<typename T>
		static inline void MoveColor_Down	( int aCount, typename T::Start& aPos, const typename T::Gradients& aGradients ) { }
		template<typename T>
		static inline void MoveUV_Right		( int aCount, typename T::Start& aPos, const typename T::Gradients& aGradients ) { }
		template<typename T>
		static inline void MoveUV_Down		( int aCount, typename T::Start& aPos, const typename T::Gradients& aGradients ) { }
	};
	
	
	// ---- //

	template<>
	static inline void RasterTile<Raster::Simple>(uint32* aData, const Raster::Simple::Start* /*aStartValue*/, const Raster::Simple::Gradients* /*aGradients*/)
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
	static inline void RasterTile<Raster::Simple>(const uint8* aMask, uint32* aData, const Raster::Simple::Start* /*aStartValue*/, const Raster::Simple::Gradients* /*aGradients*/)
	{
		const uint32 lColor = 0xFFFFFFFF;
		const uint8* lMask = aMask;
		for(uint32 i = 0; i < TILE_SIZE; ++i)
		{
			for(uint32 j = 0; j < TILE_SIZE; j++)
				if(lMask[ j] != 0) aData[ j] = lColor;
			lMask+= TILE_SIZE;
			aData+= sWidth;
		}
	}
	
	template<>
	static inline void RasterTile<Raster::VertexColor>(uint32* aData, const Raster::VertexColor::Start* aStartValue, const Raster::VertexColor::Gradients* aGradients)
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
	static inline void RasterTile<Raster::VertexColor>(const uint8* aMask, uint32* aData, const Raster::VertexColor::Start* aStartValue, const Raster::VertexColor::Gradients* aGradients)
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

	template<>
	class Gradient<Raster::Present>
	{
	public:
		template<typename T>
		static inline void GetColor( typename T::Gradients* aGradients, float aValX, float aValY, int X0, int X1, int X2, int Y0, int Y1, int Y2, float aLengthX, float aLengthY)
		{
			aGradients->mColorX = (Interpolate(aValY, sColors[Y0], sColors[Y2]) - sColors[Y1]) / aLengthY;
			aGradients->mColorY = (Interpolate(aValX, sColors[X0], sColors[X2]) - sColors[X1]) / aLengthX;
		}
		
		template<typename T>
		static inline void GetUV( typename T::Gradients* aGradients, float aValX, float aValY, int X0, int X1, int X2, int Y0, int Y1, int Y2, float aLengthX, float aLengthY)
		{
			aGradients->mUVX = (Interpolate(aValY, sUVs[Y0], sUVs[Y2]) - sUVs[Y1]) / aLengthY;
			aGradients->mUVY = (Interpolate(aValX, sUVs[X0], sUVs[X2]) - sUVs[X1]) / aLengthX;
		}
	};
	
	template<>
	class Start<Raster::Present>
	{
	public:
		template<typename T>
		static inline void GetColor( int i0, typename T::Gradients& aGradients, typename T::Start& aStarts)
		{
			aStarts.mColor = sColors[i0];
			aStarts.mColor = aStarts.mColor - (aGradients.mColorX * sPositions[i0].x);
			aStarts.mColor = aStarts.mColor - (aGradients.mColorY * sPositions[i0].y);
		}
		
		template<typename T>
		static inline void GetUV( int i0, typename T::Gradients& aGradients, typename T::Start& aStarts)
		{
			//Not implemented yet!
		}
	};

	template<>
	class TileSetup<Raster::Present>
	{
	public:
		template<typename T>
		static inline void MoveColor_Right( int aCount, typename T::Start& aPos, const typename T::Gradients& aGradients )
		{
			aPos.mColor = aPos.mColor + (aGradients.mColorX * Real(TILE_SIZE * aCount));
		}
		template<typename T>
		static inline void MoveColor_Down ( int aCount, typename T::Start& aPos, const typename T::Gradients& aGradients )
		{
			aPos.mColor = aPos.mColor + (aGradients.mColorY * Real(TILE_SIZE * aCount));
		}
		template<typename T>
		static inline void MoveUV_Right   ( int aCount, typename T::Start& aPos, const typename T::Gradients& aGradients )
		{
			//Not implemented yet!
		}
		template<typename T>
		static inline void MoveUV_Down    ( int aCount, typename T::Start& aPos, const typename T::Gradients& aGradients )
		{
			//Not implemented yet!
		}
	};

	//--[ Triangle Setup ]--//
	
	template<typename T>
	static inline void GetGradients(int i0, int i1, int i2, typename T::Gradients& aGradients)
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

		Gradient<typename T::Color>::template GetColor<T>( &aGradients, lValX, lValY
														 , X0, X1, X2
														 , Y0, Y1, Y2
														 , lSplitX - sPositions[X1].y
														 , lSplitY - sPositions[Y1].x );
		
		Gradient<typename T::UV>::template GetUV<T>( &aGradients, lValX, lValY
												   , X0, X1, X2
												   , Y0, Y1, Y2
												   , lSplitX - sPositions[X1].y
												   , lSplitY - sPositions[Y1].x );
	}
	
	template<typename T>
	static inline void GetStartPoints(int i0, typename T::Gradients& aGradients, typename T::Start& aStarts)
	{
		Start<typename T::Color>::template GetColor<T>	(i0, aGradients, aStarts);
		Start<typename T::UV>::	  template GetUV<T>		(i0, aGradients, aStarts);
	}
	
	template<typename T>
	static inline void MoveTile_Right(int aCount, typename T::Start& aTilePosition, const typename T::Gradients& aGradients)
	{
		TileSetup<typename T::Color>::template MoveColor_Right<T>(aCount, aTilePosition, aGradients);
		TileSetup<typename T::UV>::   template MoveUV_Right<T>	(aCount, aTilePosition, aGradients);
	}

	template<typename T>
	static inline void MoveTile_Down(int aCount, typename T::Start& aTilePosition, const typename T::Gradients& aGradients)
	{
		TileSetup<typename T::Color>::template MoveColor_Down<T>	(aCount, aTilePosition, aGradients);
		TileSetup<typename T::UV>::   template MoveUV_Down<T>	(aCount, aTilePosition, aGradients);
	}
	
	static inline void GenerateMask(uint8* aMask, const Position2& aTilePosition, const Position2& a0, const Position2& a1, const Position2& a2)
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
	
	template <typename T>
	void RasterTriangle(int i0, int i1, int i2)
	{
		uint32* lPtr = (uint32*)sScreen;

		typename T::Gradients lGradients;
		GetGradients<T>(i0, i1, i2, lGradients);

		typename T::Start lStart;
		GetStartPoints<T>(i0, lGradients, lStart);

		Position2 lA(sPositions[i0].x, sPositions[i0].y);
		Position2 lB(sPositions[i1].x, sPositions[i1].y);
		Position2 lC(sPositions[i2].x, sPositions[i2].y);
		Position2 lTemp;
		if(!PointInsideEdge(lC, lA, lB))
		{	// Ordeno los vertices para que A-B-C vaya en el sentido de las agujas del reloj
			lTemp = lA;
			lA = lB;
			lB = lTemp;
		}
		
		for(uint32 i = 0; i < sTilesHeight ; i++)
		{
			typename T::Start lStart2 = lStart;
			MoveTile_Down<T>(i, lStart2, lGradients);

			lPtr = (uint32*)sScreen + ((i*TILE_SIZE) * sWidth);
			for(uint32 j = 0; j < sTilesWidth; j++)
			{
				if(!TileOutsideTriangle(i * (sTilesWidth + 1) + j, lA, lB, lC))
				{
					if(TileInsideTriangle(i * (sTilesWidth + 1) + j , lA, lB, lC))
						RasterTile<T>(lPtr, &lStart2, &lGradients);
					else
					{
						// Partially inside
						uint8 lMask[TILE_SIZE * TILE_SIZE];
						GenerateMask(lMask, Position2(float(j*TILE_SIZE), float(i * TILE_SIZE)), lA, lB, lC);
						RasterTile<T>(lMask, lPtr, &lStart2, &lGradients);
					}
				}
				MoveTile_Right<T>(1, lStart2, lGradients);
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
				lPtr->x = float(TILE_SIZE * j);
				lPtr->y = float(TILE_SIZE * i);
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
		for(int i = 0; i < sNumPositions; i+= 3)
			RasterTriangle<Raster::VertexColor>(i+0,i+1,i+2);
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

