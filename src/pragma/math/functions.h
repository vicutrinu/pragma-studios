#pragma once

#include "vector3.h"

namespace pragma
{

	//---[ vector2<T> ] ---//
	template <typename T>
	inline vector2<T> operator+(const vector2<T>& aSrc1, const vector2<T>& aSrc2)
	{
		return vector2<T>(aSrc1.x + aSrc2.x, aSrc1.y + aSrc2.y);
	}

	template <typename T>
	inline vector2<T> operator-(const vector2<T>& aSrc1, const vector2<T>& aSrc2)
	{
		return vector2<T>(aSrc1.x - aSrc2.x, aSrc1.y - aSrc2.y);
	}

	template <typename T>
	inline bool operator==(const vector2<T>& aSrc1, const vector2<T>& aSrc2)
	{
		return aSrc1.x == aSrc2.x && aSrc1.y == aSrc2.y;
	}

	//---[ vector3<T> ] ---//
	template <typename T>
	inline vector3<T> operator+(const vector3<T>& aSrc1, const vector3<T>& aSrc2)
	{
		return vector3<T>(aSrc1.x + aSrc2.x, aSrc1.y + aSrc2.y, aSrc1.z + aSrc2.z);
	}

	template <typename T>
	inline vector3<T> operator-(const vector3<T>& aSrc1, const vector3<T>& aSrc2)
	{
		return vector3<T>(aSrc1.x - aSrc2.x, aSrc1.y - aSrc2.y, aSrc1.z - aSrc2.z);
	}

	template <typename T>
	inline bool operator==(const vector3<T>& aSrc1, const vector3<T>& aSrc2)
	{
		return aSrc1.x == aSrc2.x && aSrc1.y == aSrc2.y && aSrc1.z == aSrc2.z;
	}

	//---[ vector4<T> ] ---//
	template <typename T>
	inline vector4<T> operator+(const vector4<T>& aSrc1, const vector4<T>& aSrc2)
	{
		return vector4<T>(aSrc1.x + aSrc2.x, aSrc1.y + aSrc2.y, aSrc1.z + aSrc2.z, aSrc1.w + aSrc2.w);
	}

	template <typename T>
	inline vector4<T> operator-(const vector4<T>& aSrc1, const vector4<T>& aSrc2)
	{
		return vector4<T>(aSrc1.x - aSrc2.x, aSrc1.y - aSrc2.y, aSrc1.z - aSrc2.z, aSrc1.w - aSrc2.w);
	}

	template <typename T>
	inline bool operator==(const vector4<T>& aSrc1, const vector4<T>& aSrc2)
	{
		return aSrc1.x == aSrc2.x && aSrc1.y == aSrc2.y && aSrc1.z == aSrc2.z && aSrc1.w == aSrc2.w;
	}

	//---[ vector2<T> ] ---//
	template <typename T>
	inline vector2<T> operator*(const vector2<T>& aSrc, T aScale)
	{
		return vector2<T>( aSrc.x * aScale, aSrc.y * aScale);
	}

	//---[ vector3<T> ] ---//
	template <typename T>
	inline vector3<T> operator*(const vector3<T>& aSrc, T aScale)
	{
		return vector3<T>( aSrc.x * aScale, aSrc.y * aScale, aSrc.z * aScale);
	}

	template <typename T>
	inline vector3<T> CrossProduct(const vector3<T>& aSrc1, const vector3<T> aSrc2)
	{
		return vector3<T>( aSrc1.y * aSrc2.z - aSrc1.z * aSrc2.y
						 , aSrc1.z * aSrc2.x - aSrc1.x * aSrc2.z
						 , aSrc1.x * aSrc2.y - aSrc1.y * aSrc2.x );
	}

	template <typename T>
	inline T DotProduct(const vector3<T>& aSrc1, const vector3<T> aSrc2)
	{
		return aSrc1.x * aSrc2.x + aSrc1.y * aSrc2.y + aSrc1.z * aSrc2.z;
	}

	//---[ vector4<T> ] ---//
	template <typename T>
	inline vector4<T> operator*(const vector4<T>& aSrc, T aScale)
	{
		return vector4<T>( aSrc.x * aScale, aSrc.y * aScale, aSrc.z * aScale, aSrc.w * aScale);
	}

	//---[ matrix3x3<T> ] ---//
	template <typename T>
	inline matrix3x3<T> operator+(const matrix3x3<T>& aSrc1, const matrix3x3<T>& aSrc2)
	{
		return matrix3x3<T> ( aSrc1.i[0][0] + aSrc2.i[0][0], aSrc1.i[0][1] + aSrc2.i[0][1], aSrc1.i[0][2] + aSrc2.i[0][2]
							, aSrc1.i[1][0] + aSrc2.i[1][0], aSrc1.i[1][1] + aSrc2.i[1][1], aSrc1.i[1][2] + aSrc2.i[1][2]
							, aSrc1.i[2][0] + aSrc2.i[2][0], aSrc1.i[2][1] + aSrc2.i[2][1], aSrc1.i[2][2] + aSrc2.i[2][2] );
	}

	template <typename T>
	inline matrix3x3<T> operator-(const matrix3x3<T>& aSrc1, const matrix3x3<T>& aSrc2)
	{
		return matrix3x3<T> ( aSrc1.i[0][0] - aSrc2.i[0][0], aSrc1.i[0][1] - aSrc2.i[0][1], aSrc1.i[0][2] - aSrc2.i[0][2]
							, aSrc1.i[1][0] - aSrc2.i[1][0], aSrc1.i[1][1] - aSrc2.i[1][1], aSrc1.i[1][2] - aSrc2.i[1][2]
							, aSrc1.i[2][0] - aSrc2.i[2][0], aSrc1.i[2][1] - aSrc2.i[2][1], aSrc1.i[2][2] - aSrc2.i[2][2] );
	}

	template <typename T>
	inline bool operator==(const matrix3x3<T>& aSrc1, const matrix3x3<T>& aSrc2)
	{
		return aSrc1.i[0][0] == aSrc2.i[0][0] && aSrc1.i[0][1] == aSrc2.i[0][1] && aSrc1.i[0][2] == aSrc2.i[0][2] &&
			   aSrc1.i[1][0] == aSrc2.i[1][0] && aSrc1.i[1][1] == aSrc2.i[1][1] && aSrc1.i[1][2] == aSrc2.i[1][2] &&
			   aSrc1.i[2][0] == aSrc2.i[2][0] && aSrc1.i[2][1] == aSrc2.i[2][1] && aSrc1.i[2][2] == aSrc2.i[2][2] ;
	}

	template <typename T>
	inline matrix3x3<T> operator*(const matrix3x3<T>& aSrc, T aScale)
	{
		return matrix3x3<T> ( aSrc.i[0][0] * aScale, aSrc.i[0][1] * aScale, aSrc.i[0][2] * aScale
							, aSrc.i[1][0] * aScale, aSrc.i[1][1] * aScale, aSrc.i[1][2] * aScale
							, aSrc.i[2][0] * aScale, aSrc.i[2][1] * aScale, aSrc.i[2][2] * aScale );
	}

}
