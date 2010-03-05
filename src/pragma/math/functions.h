#pragma once

#include <pragma/math/math.h>
#include "vector3.h"
#include "type_traits.h"

namespace pragma
{
	
	template <typename T>
	inline vector2<T> operator-(const base_vector2<T>& aVector)
	{
		return vector2<T>(-aVector.x, -aVector.y);
	}

	template <typename T>
	inline vector3<T> operator-(const base_vector3<T>& aVector)
	{
		return vector3<T>(-aVector.x, -aVector.y, -aVector.z);
	}

	template <typename T>
	inline vector4<T> operator-(const base_vector4<T>& aVector)
	{
		return vector4<T>(-aVector.x, -aVector.y, -aVector.z, -aVector.w);
	}

	//---[ vector2<T> ] ---//
	template <typename T>
	inline base_vector2<T> operator+(const base_vector2<T>& aSrc1, const base_vector2<T>& aSrc2)
	{
		return vector2<T>(aSrc1.x + aSrc2.x, aSrc1.y + aSrc2.y);
	}

	template <typename T>
	inline base_vector2<T> operator-(const base_vector2<T>& aSrc1, const base_vector2<T>& aSrc2)
	{
		return vector2<T>(aSrc1.x - aSrc2.x, aSrc1.y - aSrc2.y);
	}

	template <typename T>
	inline bool operator==(const base_vector2<T>& aSrc1, const base_vector2<T>& aSrc2)
	{
		return aSrc1.x == aSrc2.x && aSrc1.y == aSrc2.y;
	}

	//---[ vector3<T> ] ---//
	template <typename T>
	inline base_vector3<T> operator+(const base_vector3<T>& aSrc1, const base_vector3<T>& aSrc2)
	{
		return vector3<T>(aSrc1.x + aSrc2.x, aSrc1.y + aSrc2.y, aSrc1.z + aSrc2.z);
	}

	template <typename T>
	inline base_vector3<T> operator-(const base_vector3<T>& aSrc1, const base_vector3<T>& aSrc2)
	{
		return vector3<T>(aSrc1.x - aSrc2.x, aSrc1.y - aSrc2.y, aSrc1.z - aSrc2.z);
	}

	template <typename T>
	inline bool operator==(const base_vector3<T>& aSrc1, const base_vector3<T>& aSrc2)
	{
		return aSrc1.x == aSrc2.x && aSrc1.y == aSrc2.y && aSrc1.z == aSrc2.z;
	}

	//---[ vector4<T> ] ---//
	template <typename T>
	inline base_vector4<T> operator+(const base_vector4<T>& aSrc1, const base_vector4<T>& aSrc2)
	{
		return vector4<T>(aSrc1.x + aSrc2.x, aSrc1.y + aSrc2.y, aSrc1.z + aSrc2.z, aSrc1.w + aSrc2.w);
	}

	template <typename T>
	inline base_vector4<T> operator-(const base_vector4<T>& aSrc1, const base_vector4<T>& aSrc2)
	{
		return vector4<T>(aSrc1.x - aSrc2.x, aSrc1.y - aSrc2.y, aSrc1.z - aSrc2.z, aSrc1.w - aSrc2.w);
	}

	template <typename T>
	inline bool operator==(const base_vector4<T>& aSrc1, const base_vector4<T>& aSrc2)
	{
		return aSrc1.x == aSrc2.x && aSrc1.y == aSrc2.y && aSrc1.z == aSrc2.z && aSrc1.w == aSrc2.w;
	}

	//---[ vector2<T> ] ---//
	template <typename T>
	inline base_vector2<T> operator*(const base_vector2<T>& aSrc, T aScale)
	{
		return vector2<T>( aSrc.x * aScale, aSrc.y * aScale);
	}
	
	template <typename T>
	inline base_vector2<T> operator*(T aScale, const base_vector2<T>& aSrc)
	{
		return vector2<T>( aSrc.x * aScale, aSrc.y * aScale);
	}

	template <typename T, typename T2>
	inline base_vector2<T> operator/(const base_vector2<T>& aSrc, T2 aInvScale)
	{
		return  aSrc * (T(1) / aInvScale);
	}

	//---[ vector3<T> ] ---//
	template <typename T>
	inline base_vector3<T> operator*(const base_vector3<T>& aSrc1, const base_vector3<T> aSrc2)
	{
		return vector3<T>( aSrc1.x * aSrc2.x, aSrc1.y * aSrc2.y, aSrc1.z * aSrc2.z );
	}

	template <typename T>
	inline base_vector3<T> operator*(const base_vector3<T>& aSrc, T aScale)
	{
		return vector3<T>( aSrc.x * aScale, aSrc.y * aScale, aSrc.z * aScale);
	}
	
	template <typename T>
	inline base_vector3<T> operator*(T aScale, const base_vector3<T>& aSrc)
	{
		return vector3<T>( aSrc.x * aScale, aSrc.y * aScale, aSrc.z * aScale);
	}

	template <typename T, typename T2>
	inline base_vector3<T> operator/(const base_vector3<T>& aSrc, T2 aInvScale)
	{
		return  aSrc * (T(1) / aInvScale);
	}

	template <typename T>
	inline base_vector3<T> CrossProduct(const base_vector3<T>& aSrc1, const base_vector3<T> aSrc2)
	{
		return vector3<T>( (aSrc1.y * aSrc2.z) - (aSrc1.z * aSrc2.y)
						 , (aSrc1.z * aSrc2.x) - (aSrc1.x * aSrc2.z)
						 , (aSrc1.x * aSrc2.y) - (aSrc1.y * aSrc2.x) );
	}

	template <typename T>
	inline T DotProduct(const base_vector3<T>& aSrc1, const base_vector3<T> aSrc2)
	{
		return aSrc1.x * aSrc2.x + aSrc1.y * aSrc2.y + aSrc1.z * aSrc2.z;
	}

	template <typename T>
	inline T DotProduct(const base_vector4<T>& aSrc1, const base_vector4<T> aSrc2)
	{
		return aSrc1.x * aSrc2.x + aSrc1.y * aSrc2.y + aSrc1.z * aSrc2.z + aSrc1.w * aSrc2.w;
	}

	//---[ vector4<T> ] ---//
	template <typename T>
	inline base_vector4<T> operator*(const base_vector4<T>& aSrc, T aScale)
	{
		return vector4<T>( aSrc.x * aScale, aSrc.y * aScale, aSrc.z * aScale, aSrc.w * aScale);
	}
	
	template <typename T>
	inline base_vector4<T> operator*(T aScale, const base_vector4<T>& aSrc)
	{
		return vector4<T>( aSrc.x * aScale, aSrc.y * aScale, aSrc.z * aScale, aSrc.w * aScale);
	}

	template <typename T, typename T2>
	inline base_vector4<T> operator/(const base_vector4<T>& aSrc, T2 aInvScale)
	{
		return aSrc * (T(1) / aInvScale);
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

	template <typename T>
	inline matrix4x4<T> operator*(const matrix4x4<T>& aSrc, T aScale)
	{
		return matrix4x4<T> ( aSrc.i[0][0] * aScale, aSrc.i[0][1] * aScale, aSrc.i[0][2] * aScale, aSrc.i[0][3] * aScale
							, aSrc.i[1][0] * aScale, aSrc.i[1][1] * aScale, aSrc.i[1][2] * aScale, aSrc.i[1][3] * aScale
							, aSrc.i[2][0] * aScale, aSrc.i[2][1] * aScale, aSrc.i[2][2] * aScale, aSrc.i[2][3] * aScale 
							, aSrc.i[3][0] * aScale, aSrc.i[3][1] * aScale, aSrc.i[3][2] * aScale, aSrc.i[3][3] * aScale );
	}

	template <typename T>
	inline matrix3x3<T> operator*(const matrix3x3<T>& aSrc1, const matrix3x3<T>& aSrc2)
	{
		return matrix3x3<T> ( DotProduct<T>(aSrc1.mRow0, vector3<T>(aSrc2.i[0][0], aSrc2.i[1][0], aSrc2.i[2][0]))
							, DotProduct<T>(aSrc1.mRow0, vector3<T>(aSrc2.i[0][1], aSrc2.i[1][1], aSrc2.i[2][1]))
							, DotProduct<T>(aSrc1.mRow0, vector3<T>(aSrc2.i[0][2], aSrc2.i[1][2], aSrc2.i[2][2]))
							, DotProduct<T>(aSrc1.mRow1, vector3<T>(aSrc2.i[0][0], aSrc2.i[1][0], aSrc2.i[2][0]))
							, DotProduct<T>(aSrc1.mRow1, vector3<T>(aSrc2.i[0][1], aSrc2.i[1][1], aSrc2.i[2][1]))
							, DotProduct<T>(aSrc1.mRow1, vector3<T>(aSrc2.i[0][2], aSrc2.i[1][2], aSrc2.i[2][2]))
							, DotProduct<T>(aSrc1.mRow2, vector3<T>(aSrc2.i[0][0], aSrc2.i[1][0], aSrc2.i[2][0]))
							, DotProduct<T>(aSrc1.mRow2, vector3<T>(aSrc2.i[0][1], aSrc2.i[1][1], aSrc2.i[2][1]))
							, DotProduct<T>(aSrc1.mRow2, vector3<T>(aSrc2.i[0][2], aSrc2.i[1][2], aSrc2.i[2][2]))
							);							
	}

	template <typename T>
	inline matrix4x4<T> operator*(const matrix4x4<T>& aSrc1, const matrix4x4<T>& aSrc2)
	{
		return matrix4x4<T> ( DotProduct<T>(aSrc1.mRow0, vector4<T>(aSrc2.i[0][0], aSrc2.i[1][0], aSrc2.i[2][0], aSrc2.i[3][0]))
							, DotProduct<T>(aSrc1.mRow0, vector4<T>(aSrc2.i[0][1], aSrc2.i[1][1], aSrc2.i[2][1], aSrc2.i[3][1]))
							, DotProduct<T>(aSrc1.mRow0, vector4<T>(aSrc2.i[0][2], aSrc2.i[1][2], aSrc2.i[2][2], aSrc2.i[3][2]))
							, DotProduct<T>(aSrc1.mRow0, vector4<T>(aSrc2.i[0][3], aSrc2.i[1][3], aSrc2.i[2][3], aSrc2.i[3][3]))
							, DotProduct<T>(aSrc1.mRow1, vector4<T>(aSrc2.i[0][0], aSrc2.i[1][0], aSrc2.i[2][0], aSrc2.i[3][0]))
							, DotProduct<T>(aSrc1.mRow1, vector4<T>(aSrc2.i[0][1], aSrc2.i[1][1], aSrc2.i[2][1], aSrc2.i[3][1]))
							, DotProduct<T>(aSrc1.mRow1, vector4<T>(aSrc2.i[0][2], aSrc2.i[1][2], aSrc2.i[2][2], aSrc2.i[3][2]))
							, DotProduct<T>(aSrc1.mRow1, vector4<T>(aSrc2.i[0][3], aSrc2.i[1][3], aSrc2.i[2][3], aSrc2.i[3][3]))
							, DotProduct<T>(aSrc1.mRow2, vector4<T>(aSrc2.i[0][0], aSrc2.i[1][0], aSrc2.i[2][0], aSrc2.i[3][0]))
							, DotProduct<T>(aSrc1.mRow2, vector4<T>(aSrc2.i[0][1], aSrc2.i[1][1], aSrc2.i[2][1], aSrc2.i[3][1]))
							, DotProduct<T>(aSrc1.mRow2, vector4<T>(aSrc2.i[0][2], aSrc2.i[1][2], aSrc2.i[2][2], aSrc2.i[3][2]))
							, DotProduct<T>(aSrc1.mRow2, vector4<T>(aSrc2.i[0][3], aSrc2.i[1][3], aSrc2.i[2][3], aSrc2.i[3][3]))
							, DotProduct<T>(aSrc1.mRow3, vector4<T>(aSrc2.i[0][0], aSrc2.i[1][0], aSrc2.i[2][0], aSrc2.i[3][0]))
							, DotProduct<T>(aSrc1.mRow3, vector4<T>(aSrc2.i[0][1], aSrc2.i[1][1], aSrc2.i[2][1], aSrc2.i[3][1]))
							, DotProduct<T>(aSrc1.mRow3, vector4<T>(aSrc2.i[0][2], aSrc2.i[1][2], aSrc2.i[2][2], aSrc2.i[3][2]))
							, DotProduct<T>(aSrc1.mRow3, vector4<T>(aSrc2.i[0][3], aSrc2.i[1][3], aSrc2.i[2][3], aSrc2.i[3][3]))
							);							
	}

	template <typename T>
	inline T Determinant(const matrix3x3<T>& aSrc)
	{
		return (aSrc.i[0][0] * aSrc.i[1][1] * aSrc.i[2][2]) +
			   (aSrc.i[0][1] * aSrc.i[1][2] * aSrc.i[2][0]) +
			   (aSrc.i[0][2] * aSrc.i[1][0] * aSrc.i[2][1]) -
			   (aSrc.i[0][0] * aSrc.i[1][2] * aSrc.i[2][1]) -
			   (aSrc.i[0][1] * aSrc.i[1][0] * aSrc.i[2][2]) -
			   (aSrc.i[0][2] * aSrc.i[1][1] * aSrc.i[2][0]);
	}

	template <typename T>
	inline matrix4x4<T> Inverse(const matrix4x4<T>& aSrc)
	{
		T fA0 = aSrc.i[0][0]*aSrc.i[1][1] - aSrc.i[0][1]*aSrc.i[1][0];
		T fA1 = aSrc.i[0][0]*aSrc.i[1][2] - aSrc.i[0][2]*aSrc.i[1][0];
		T fA2 = aSrc.i[0][0]*aSrc.i[1][3] - aSrc.i[0][3]*aSrc.i[1][0];
		T fA3 = aSrc.i[0][1]*aSrc.i[1][2] - aSrc.i[0][2]*aSrc.i[1][1];
		T fA4 = aSrc.i[0][1]*aSrc.i[1][3] - aSrc.i[0][3]*aSrc.i[1][1];
		T fA5 = aSrc.i[0][2]*aSrc.i[1][3] - aSrc.i[0][3]*aSrc.i[1][2];
		T fB0 = aSrc.i[2][0]*aSrc.i[3][1] - aSrc.i[2][1]*aSrc.i[3][0];
		T fB1 = aSrc.i[2][0]*aSrc.i[3][2] - aSrc.i[2][2]*aSrc.i[3][0];
		T fB2 = aSrc.i[2][0]*aSrc.i[3][3] - aSrc.i[2][3]*aSrc.i[3][0];
		T fB3 = aSrc.i[2][1]*aSrc.i[3][2] - aSrc.i[2][2]*aSrc.i[3][1];
		T fB4 = aSrc.i[2][1]*aSrc.i[3][3] - aSrc.i[2][3]*aSrc.i[3][1];
		T fB5 = aSrc.i[2][2]*aSrc.i[3][3] - aSrc.i[2][3]*aSrc.i[3][2];

		T fDet = fA0*fB5-fA1*fB4+fA2*fB3+fA3*fB2-fA4*fB1+fA5*fB0;
		if (abs<T>(fDet) <= math::type_traits<T>::epsilon )
		{
			return matrix4x4<T>(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
		}

		matrix4x4<T> kInv;
		kInv.i[0][0] = + aSrc.i[1][1]*fB5 - aSrc.i[1][2]*fB4 + aSrc.i[1][3]*fB3;
		kInv.i[1][0] = - aSrc.i[1][0]*fB5 + aSrc.i[1][2]*fB2 - aSrc.i[1][3]*fB1;
		kInv.i[2][0] = + aSrc.i[1][0]*fB4 - aSrc.i[1][1]*fB2 + aSrc.i[1][3]*fB0;
		kInv.i[3][0] = - aSrc.i[1][0]*fB3 + aSrc.i[1][1]*fB1 - aSrc.i[1][2]*fB0;
		kInv.i[0][1] = - aSrc.i[0][1]*fB5 + aSrc.i[0][2]*fB4 - aSrc.i[0][3]*fB3;
		kInv.i[1][1] = + aSrc.i[0][0]*fB5 - aSrc.i[0][2]*fB2 + aSrc.i[0][3]*fB1;
		kInv.i[2][1] = - aSrc.i[0][0]*fB4 + aSrc.i[0][1]*fB2 - aSrc.i[0][3]*fB0;
		kInv.i[3][1] = + aSrc.i[0][0]*fB3 - aSrc.i[0][1]*fB1 + aSrc.i[0][2]*fB0;
		kInv.i[0][2] = + aSrc.i[3][1]*fA5 - aSrc.i[3][2]*fA4 + aSrc.i[3][3]*fA3;
		kInv.i[1][2] = - aSrc.i[3][0]*fA5 + aSrc.i[3][2]*fA2 - aSrc.i[3][3]*fA1;
		kInv.i[2][2] = + aSrc.i[3][0]*fA4 - aSrc.i[3][1]*fA2 + aSrc.i[3][3]*fA0;
		kInv.i[3][2] = - aSrc.i[3][0]*fA3 + aSrc.i[3][1]*fA1 - aSrc.i[3][2]*fA0;
		kInv.i[0][3] = - aSrc.i[2][1]*fA5 + aSrc.i[2][2]*fA4 - aSrc.i[2][3]*fA3;
		kInv.i[1][3] = + aSrc.i[2][0]*fA5 - aSrc.i[2][2]*fA2 + aSrc.i[2][3]*fA1;
		kInv.i[2][3] = - aSrc.i[2][0]*fA4 + aSrc.i[2][1]*fA2 - aSrc.i[2][3]*fA0;
		kInv.i[3][3] = + aSrc.i[2][0]*fA3 - aSrc.i[2][1]*fA1 + aSrc.i[2][2]*fA0;

		T fInvDet = T(1)/fDet;
		kInv = kInv * fInvDet;

		return kInv;
	}

	template <typename T>
	inline base_vector3<T> TransformPoint(const matrix3x3<T>& aMatrix, const base_vector3<T>& aColumnVector)
	{
		return vector3<T>( DotProduct<T>(aMatrix.mRow0, aColumnVector)
						 , DotProduct<T>(aMatrix.mRow1, aColumnVector)
						 , DotProduct<T>(aMatrix.mRow2, aColumnVector) );
	}

	template <typename T>
	inline base_vector3<T> TransformPoint(const base_vector3<T>& aVector, const matrix3x3<T>& aMatrix)
	{
		return vector3<T>( DotProduct<T>( aVector, vector3<T>(aMatrix.i[0][0], aMatrix.i[0][1], aMatrix.i[0][2] ) )
						 , DotProduct<T>( aVector, vector3<T>(aMatrix.i[0][0], aMatrix.i[0][1], aMatrix.i[0][2] ) )
						 , DotProduct<T>( aVector, vector3<T>(aMatrix.i[0][0], aMatrix.i[0][1], aMatrix.i[0][2] )) );
	}

	template <typename T>
	inline base_vector4<T> TransformPoint(const matrix4x4<T>& aMatrix, const base_vector4<T>& aColumnVector)
	{
		return vector4<T>( DotProduct<T>(aMatrix.mRow0, aColumnVector)
						 , DotProduct<T>(aMatrix.mRow1, aColumnVector)
						 , DotProduct<T>(aMatrix.mRow2, aColumnVector)
						 , DotProduct<T>(aMatrix.mRow3, aColumnVector) );
	}

	template <typename T>
	inline base_vector4<T> TransformPoint(const base_vector4<T>& aVector, const matrix4x4<T>& aMatrix)
	{
		return vector4<T>( DotProduct<T>( aVector, vector4<T>(aMatrix.i[0][0], aMatrix.i[0][1], aMatrix.i[0][2], aMatrix.i[0][3]) )
						 , DotProduct<T>( aVector, vector4<T>(aMatrix.i[0][0], aMatrix.i[0][1], aMatrix.i[0][2], aMatrix.i[0][3]) )
						 , DotProduct<T>( aVector, vector4<T>(aMatrix.i[0][0], aMatrix.i[0][1], aMatrix.i[0][2], aMatrix.i[0][3]) )
						 , DotProduct<T>( aVector, vector4<T>(aMatrix.i[0][0], aMatrix.i[0][1], aMatrix.i[0][2], aMatrix.i[0][3]) ) );
	}

}
