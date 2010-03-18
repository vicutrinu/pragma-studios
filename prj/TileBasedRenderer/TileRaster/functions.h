#pragma once

namespace pragma { namespace TileRaster
{

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
		return (lA.x * lB.y - lA.y * lB.x) <= 0;
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

} }
