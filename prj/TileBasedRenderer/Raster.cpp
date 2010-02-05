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

namespace pragma
{
	namespace Raster
	{

		static const int sHeight = 768;
		static const int sWidth = 1024;
		
		struct Scan
		{
			Point Start;
			Point End;
		};
		
		Point sScanLeft[sHeight];
		Point sScanRight[sHeight];
	
		inline unsigned PreRunEdge(Point& aStart, const _Vector& aEdge1, const _Vector& aDir)
		{
			Real lY = int(aStart.y + Real(0.5)) - Real(0.5);
			Real lVal = (lY - aStart.y) / aDir.y; // Valor magico
			aStart = aStart + (aDir * lVal);
			
			_Point lEnd = aStart + aEdge1;
			lY = int(lEnd.y - Real(0.5)) + Real(0.5);
			lVal = (lY - lEnd.y) / aDir.y; // Valor magico
			lEnd = lEnd + (aDir * lVal);
			
			int lStartY = int(aStart.y);
			int lEndY = int(lEnd.y);
			return lEndY - lStartY;
		}
		
		inline void RunEdge(Point* aScan, _Point aStart, const _Vector& aDir, unsigned aCount)
		{
			while( aCount-- )
			{
				aScan->x = int(aStart.x - Real(0.5)) + Real(0.5);
				aScan->y = aStart.y;
				aStart+= aDir;
			}
		}
		
		void RasterTrapezoid1(const _Point& aStart, const _Vector& aLeftEdge, const _Vector& aRightEdge)
		{
			// Interpolamos el lado izquierdo
			_Vector lDir = Normalize(aLeftEdge);
			_Point lStart = aStart;
			int lLeftRowCount = PreRunEdge(lStart, aLeftEdge, lDir);
			if(lLeftRowCount == 0)
				return;
			int lStartY = int(lStart.y);
			RunEdge(&sScanLeft[lStartY], lStart, lDir, lLeftRowCount);
			
			// Interpolamos el lado derecho
			lDir = Normalize(aRightEdge);
			lStart = aStart;
			int lRightRowCount = PreRunEdge(lStart, aRightEdge, lDir);
			lStartY = int(lStart.y);
			RunEdge(&sScanRight[lStartY], lStart, lDir, lRightRowCount);
		}
		
		void RasterTrapezoid2(const Point& aStart, const Vector& aEdge1, const Vector& aEdge2)
		{
			
		}
	}
	void RasterTriangle(const Point& aV0, const Point& aV1, const Point& aV2)
	{
		//Ordenar los vertices segun la altura, para partir el triangulo en 2 trapezoides
		if(aV0.y < aV1.y)
		{
			if(aV0.y < aV2.y)
			{
				if(aV1.y < aV2.y)
				{ // 0, 1, 2
					/*Point lPoint = FindPointAt_Y(aV0, aV2, aV1.y);
					RasterTrapezoid1*/
				}
				else
				{ // 0, 2, 1
					
				}

			}
			else
			{ // 2, 0, 1

			}
		}
		else
		{
			if(aV1.y < aV2.y)
			{
				if(aV2.y < aV0.y)
				{ // 1, 2, 0
					
				}
				else
				{ // 1, 0, 2
					
				}
			}
			else
			{ // 2, 1, 0
				
			}

		}

	}
}