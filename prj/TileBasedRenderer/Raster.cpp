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

namespace pragma
{
	namespace Raster
	{

		static const int sHeight = 768;
		static const int sWidth = 1024;
		static unsigned char sScreen[sHeight * sWidth];
		
		struct Scan
		{
			Point Start;
			Point End;
		};
		
		Point sScanLeft[sHeight];
		Point sScanRight[sHeight];
	
		inline unsigned PreRunEdgeY(Point& aStart, const _Vector& aEdge)
		{
			Real lY = int(aStart.y + Real(0.5)) + Real(0.5);
			Real lVal = (lY - aStart.y) / aEdge.y; // Valor magico
			Point lStart = aStart + (aEdge * lVal);
			
			Real lEndY = aStart.y + aEdge.y;
			lEndY = int(lEndY + Real(0.5)) + Real(0.5);

			aStart = lStart;
			return int(lEndY - lStart.y);
		}
		
		inline unsigned PreRunEdgeX(Point& aStart, const _Vector& aEdge)
		{
			Real lX = int(aStart.x + Real(0.5)) + Real(0.5);
			Real lVal = (lX - aStart.x) / aEdge.x; // Valor magico
			Point lStart = aStart + (aEdge * lVal);
			
			Real lEndX = aStart.x + aEdge.x;
			lEndX = int(lEndX + Real(0.5)) + Real(0.5);
			
			aStart = lStart;
			return int(lEndX - lStart.x);
		}
		
		inline void RunEdges( Point* aLeftScan, _Point aLeftStart, Real aLeftIncrement
							, Point* aRightScan, _Point aRightStart, Real aRightIncrement
							, unsigned aCount )
		{
			while( aCount-- )
			{
				aLeftScan->x = aLeftStart.x;
				aLeftScan->y = aLeftStart.y;
				aRightScan->x = aRightStart.x;
				aRightScan->y = aRightStart.y;
				aLeftStart.x+= aLeftIncrement;
				aLeftStart.y+= 1;
				aRightStart.x+= aRightIncrement;
				aRightStart.y+= 1;
				
				unsigned lCount = PreRunEdgeX(*aLeftScan, *aRightScan-*aLeftScan);
				unsigned char* lPtr = &sScreen[int(aLeftScan->y) * 1024 + int(aLeftScan->x)];
				while(lCount--)
				{
					*lPtr++ = 255;
				}
			}
		}
		
		void RasterTrapezoid1(const _Point& aStart, const _Vector& aLeftEdge, const _Vector& aRightEdge)
		{
			// Interpolamos el lado izquierdo
			_Point lLeftStart = aStart;
			int lLeftRowCount = PreRunEdgeY(lLeftStart, aLeftEdge);

			// Interpolamos el lado derecho
			_Point lRightStart = aStart;
			int lRightRowCount = PreRunEdgeY(lRightStart, aRightEdge);
			if(lLeftRowCount != lRightRowCount)
				return;
			
			RunEdges( &sScanLeft [int(lLeftStart.y)],  lLeftStart , aLeftEdge.x  / aLeftEdge.y
					, &sScanRight[int(lRightStart.y)], lRightStart, aRightEdge.x / aRightEdge.y
					, lLeftRowCount );
		}
		
		void RasterTrapezoid2(const Point& aStart, const Vector& aLeftEdge, const Vector& aRightEdge)
		{
			// Interpolamos el lado izquierdo
			_Point lLeftStart = aStart + aLeftEdge;
			int lLeftRowCount = PreRunEdgeY(lLeftStart, -aLeftEdge);
			
			// Interpolamos el lado derecho
			_Point lRightStart = aStart + aRightEdge;
			int lRightRowCount = PreRunEdgeY(lRightStart, -aRightEdge);
			if(lLeftRowCount != lRightRowCount)
				return;
			
			RunEdges( &sScanLeft [int(lLeftStart.y)],  lLeftStart , aLeftEdge.x  / aLeftEdge.y
					, &sScanRight[int(lRightStart.y)], lRightStart, aRightEdge.x / aRightEdge.y
					, lLeftRowCount );
		}
	}
	
	void RasterTriangle(const Point& aV0, const Point& aV1, const Point& aV2)
	{
		memset(Raster::sScreen, 64, Raster::sWidth * Raster::sHeight);
		//Ordenar los vertices segun la altura, para partir el triangulo en 2 trapezoides
		if(aV0.y < aV1.y)
		{
			if(aV0.y < aV2.y)
			{
				if(aV1.y < aV2.y)
				{ // 0, 1, 2
					Vector lEdge = aV2 - aV0;
					Real lVal = (aV1.y - aV0.y) / (lEdge.y); // Valor magico
					Vector lSplit = aV0 + (lEdge * lVal); 
					if(aV1.x < aV2.x)
					{ // 1 está a la izquierda
						Raster::RasterTrapezoid1(aV0, aV1 - aV0, lSplit - aV0);
						Raster::RasterTrapezoid2(aV2, aV1 - aV2, lSplit - aV2);
					}
					else
					{ // 1 está a la derecha
						Raster::RasterTrapezoid1(aV0, lSplit - aV0, aV1 - aV0);
						Raster::RasterTrapezoid2(aV2, lSplit - aV2, aV1 - aV2);
					}
				}
				else
				{ // 0, 2, 1
					Vector lEdge = aV1 - aV0;
					Real lVal = (aV2.y - aV0.y) / (lEdge.y); // Valor magico
					Vector lSplit = aV0 + (lEdge * lVal); 
					if(aV2.x < aV1.x)
					{ // 2 está a la izquierda
						Raster::RasterTrapezoid1(aV0, aV2 - aV0, lSplit - aV0);
						Raster::RasterTrapezoid2(aV1, aV2 - aV1, lSplit - aV2);
					}
					else
					{ // 2 está a la derecha
						Raster::RasterTrapezoid1(aV0, lSplit - aV0, aV2 - aV0);
						Raster::RasterTrapezoid2(aV1, lSplit - aV1, aV2 - aV1);
					}
				}
			}
			else
			{ // 2, 0, 1
				Vector lEdge = aV1 - aV2;
				Real lVal = (aV0.y - aV2.y) / (lEdge.y); // Valor magico
				Vector lSplit = aV2 + (lEdge * lVal); 
				if(aV0.x < aV1.x)
				{ // 0 está a la izquierda
					Raster::RasterTrapezoid1(aV2, aV0 - aV2, lSplit - aV2);
					Raster::RasterTrapezoid2(aV1, aV0 - aV1, lSplit - aV1);
				}
				else
				{ // 0 está a la derecha
					Raster::RasterTrapezoid1(aV2, lSplit - aV2, aV0 - aV2);
					Raster::RasterTrapezoid2(aV1, lSplit - aV1, aV0 - aV1);
				}
			}
		}
		else
		{
			if(aV1.y < aV2.y)
			{
				if(aV2.y < aV0.y)
				{ // 1, 2, 0
					Vector lEdge = aV0 - aV1;
					Real lVal = (aV2.y - aV1.y) / (lEdge.y); // Valor magico
					Vector lSplit = aV1 + (lEdge * lVal); 
					if(aV2.x < aV0.x)
					{ // 2 está a la izquierda
						Raster::RasterTrapezoid1(aV1, aV2 - aV1, lSplit - aV1);
						Raster::RasterTrapezoid2(aV0, aV2 - aV0, lSplit - aV0);
					}
					else
					{ // 2 está a la derecha
						Raster::RasterTrapezoid1(aV1, lSplit - aV1, aV2 - aV1);
						Raster::RasterTrapezoid2(aV0, lSplit - aV0, aV2 - aV0);
					}
				}
				else
				{ // 1, 0, 2
					Vector lEdge = aV2 - aV1;
					Real lVal = (aV0.y - aV1.y) / (lEdge.y); // Valor magico
					Vector lSplit = aV1 + (lEdge * lVal); 
					if(aV0.x < aV2.x)
					{ // 0 está a la izquierda
						Raster::RasterTrapezoid1(aV1, aV0 - aV1, lSplit - aV1);
						Raster::RasterTrapezoid2(aV2, aV0 - aV2, lSplit - aV2);
					}
					else
					{ // 0 está a la derecha
						Raster::RasterTrapezoid1(aV1, lSplit - aV1, aV0 - aV1);
						Raster::RasterTrapezoid2(aV2, lSplit - aV2, aV0 - aV2);
					}
				}
			}
			else
			{ // 2, 1, 0
				Vector lEdge = aV0 - aV2;
				Real lVal = (aV1.y - aV2.y) / (lEdge.y); // Valor magico
				Vector lSplit = aV2 + (lEdge * lVal); 
				if(aV1.x < aV0.x)
				{ // 1 está a la izquierda
					Raster::RasterTrapezoid1(aV2, aV1 - aV2, lSplit - aV2);
					Raster::RasterTrapezoid2(aV0, aV1 - aV0, lSplit - aV0);
				}
				else
				{ // 1 está a la derecha
					Raster::RasterTrapezoid1(aV2, lSplit - aV2, aV1 - aV2);
					Raster::RasterTrapezoid2(aV0, lSplit - aV0, aV1 - aV0);
				}
			}

		}
		FILE* handle = fopen("out.raw", "wb");
		fwrite(Raster::sScreen, 1, Raster::sWidth * Raster::sHeight, handle);
		fclose(handle);
	}
}