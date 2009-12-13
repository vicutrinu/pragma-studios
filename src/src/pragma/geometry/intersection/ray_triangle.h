/**
 *	@brief	Calculos de las intersecciones entre un rayo y un triangulo.
 *			Sabores: Rayo con o sin longitud maxima
 *					 Triangulo con una o dos caras (backface culling)
 */
#pragma once

#include "pragma/math/functions.h"
#include "pragma/math/type_traits.h"

namespace pragma
{
	template<typename T>
	bool IntersectRayTriangle( const vector3<T>& aV0, const vector3<T>& aV1, const vector3<T>& aV2
							 , const vector3<T>& aRayOrigin, const vector3<T>& aRayDir
							 , vector2<T>& aBarycentric, T& aDistance )
	{
		vector3<T> lEdge0 = aV1 - aV0;
		vector3<T> lEdge1 = aV2 - aV0;

		vector3<T> lP = CrossProduct(aRayDir, lEdge1);

		// This is part of the determinant calculation
		T lDet = DotProduct(lEdge0, lP);

		// If lDet is near 0, the ray lies in the plane of the triangle
		if(lDet < math::type_traits<T>::epsilon)
			return false;

		vector3<T> lT = aRayOrigin - aV0;

		// Calculate first component of barycentric coordinates, and test bounds
		aBarycentric.x = DotProduct(lT, lP);
		if(aBarycentric.x < 0 || aBarycentric.x > lDet)
			return false;

		vector3f lQ = CrossProduct(lT, lEdge0);

		// Calculate second component of barycentric coordinates, and test bounds
		aBarycentric.y = DotProduct(aRayDir, lQ);
		if(aBarycentric.y < 0 || (aBarycentric.x + aBarycentric.y) > lDet)
			return false;

		// Scale and return final values
		aDistance = DotProduct(lEdge1, lQ);
		T lInv_det = T(1) / lDet;
		aDistance*= lInv_det;
		aBarycentric = aBarycentric * lInv_det;
		return true;
	}

	template<typename T>
	bool IntersectRayTriangle( const vector3<T>& aV0, const vector3<T>& aV1, const vector3<T>& aV2
							 , const vector3<T>& aRayOrigin, const vector3<T>& aRayDir, T aRayLength
							 , vector2<T>& aBarycentric, T& aDistance )
	{
		vector3<T> lEdge0 = aV1 - aV0;
		vector3<T> lEdge1 = aV2 - aV0;

		vector3<T> lP = CrossProduct(aRayDir, lEdge1);

		// This is part of the determinant calculation
		T lDet = DotProduct(lEdge0, lP);

		// If lDet is near 0, the ray lies in the plane of the triangle
		if(lDet < math::type_traits<T>::epsilon)
			return false;

		vector3<T> lT = aRayOrigin - aV0;

		// Distance test
		vector3f lQ = CrossProduct(lT, lEdge0);
		aDistance = DotProduct(lEdge1, lQ);
		T lInv_det = T(1) / lDet;
		aDistance*= lInv_det;
		if(aDistance > aRayLength)
			return false;

		// Calculate first component of barycentric coordinates, and test bounds
		aBarycentric.x = DotProduct(lT, lP);
		if(aBarycentric.x < 0 || aBarycentric.x > lDet)
			return false;

		// Calculate second component of barycentric coordinates, and test bounds
		aBarycentric.y = DotProduct(aRayDir, lQ);
		if(aBarycentric.y < 0 || (aBarycentric.x + aBarycentric.y) > lDet)
			return false;

		// Scale and return final values
		aBarycentric = aBarycentric * lInv_det;
		return true;
	}

	template<typename T>
	bool IntersectRayTriangle( const vector3<T>& aV0, const vector3<T>& aV1, const vector3<T>& aV2
							 , const vector3<T>& aRayOrigin, const vector3<T>& aRayDir )
	{
		vector3<T> lEdge0 = aV1 - aV0;
		vector3<T> lEdge1 = aV2 - aV0;

		vector3<T> lP = CrossProduct(aRayDir, lEdge1);

		// This is part of the determinant calculation
		T lDet = DotProduct(lEdge0, lP);

		// If lDet is near 0, the ray lies in the plane of the triangle
		if(lDet < math::type_traits<T>::epsilon)
			return false;

		vector3<T> lT = aRayOrigin - aV0;

		// Calculate first component of barycentric coordinates, and test bounds
		T u = DotProduct(lT, lP);
		if(u < 0 || u > lDet)
			return false;

		vector3f lQ = CrossProduct(lT, lEdge0);

		// Calculate second component of barycentric coordinates, and test bounds
		T v = DotProduct(aRayDir, lQ);
		if(v < 0 || (u + v) > lDet)
			return false;
		
		return true;
	}

	template<typename T>
	bool IntersectRayTriangle_2Sided( const vector3<T>& aV0, const vector3<T>& aV1, const vector3<T>& aV2
									, const vector3<T>& aRayOrigin, const vector3<T>& aRayDir
									, vector2<T>& aBarycentric, T& aDistance )
	{
		vector3<T> lEdge0 = aV1 - aV0;
		vector3<T> lEdge1 = aV2 - aV0;

		vector3<T> lP = CrossProduct(aRayDir, lEdge1);

		// This is part of the determinant calculation
		T lDet = DotProduct(lEdge0, lP);

		// If lDet is near 0, the ray lies in the plane of the triangle
		if(lDet > -math::type_traits<T>::epsilon && lDet < math::type_traits<T>::epsilon)
			return false;

		T lInvDet = T(1) / lDet;

		vector3<T> lT = aRayOrigin - aV0;

		// Calculate first component of barycentric coordinates, and test bounds
		aBarycentric.x = DotProduct(lT, lP) * lInvDet;
		if(aBarycentric.x < T(0) || aBarycentric.x > T(1))
			return false;

		vector3f lQ = CrossProduct(lT, lEdge0);

		// Calculate second component of barycentric coordinates, and test bounds
		aBarycentric.y = DotProduct(aRayDir, lQ) * lInvDet;
		if(aBarycentric.y < T(0) || (aBarycentric.x + aBarycentric.y) > T(1))
			return false;

		// Scale and return final values
		aDistance = DotProduct(lEdge1, lQ) * lInvDet;
		return true;
	}

	template<typename T>
	bool IntersectRayTriangle_2Sided( const vector3<T>& aV0, const vector3<T>& aV1, const vector3<T>& aV2
									, const vector3<T>& aRayOrigin, const vector3<T>& aRayDir, T aRayLength
									, vector2<T>& aBarycentric, T& aDistance )
	{
		vector3<T> lEdge0 = aV1 - aV0;
		vector3<T> lEdge1 = aV2 - aV0;

		vector3<T> lP = CrossProduct(aRayDir, lEdge1);

		// This is part of the determinant calculation
		T lDet = DotProduct(lEdge0, lP);

		// If lDet is near 0, the ray lies in the plane of the triangle
		if(lDet > -math::type_traits<T>::epsilon && lDet < math::type_traits<T>::epsilon)
			return false;

		T lInvDet = T(1) / lDet;

		vector3<T> lT = aRayOrigin - aV0;
		vector3f lQ = CrossProduct(lT, lEdge0);
		aDistance = DotProduct(lEdge1, lQ) * lInvDet;
		if(aDistance > aRayLength)
			return false;

		// Calculate first component of barycentric coordinates, and test bounds
		aBarycentric.x = DotProduct(lT, lP) * lInvDet;
		if(aBarycentric.x < T(0) || aBarycentric.x > T(1))
			return false;

		// Calculate second component of barycentric coordinates, and test bounds
		aBarycentric.y = DotProduct(aRayDir, lQ) * lInvDet;
		if(aBarycentric.y < T(0) || (aBarycentric.x + aBarycentric.y) > T(1))
			return false;

		return true;
	}

	template<typename T>
	bool IntersectRayTriangle_2Sided( const vector3<T>& aV0, const vector3<T>& aV1, const vector3<T>& aV2
									, const vector3<T>& aRayOrigin, const vector3<T>& aRayDir )
	{
		vector3<T> lEdge0 = aV1 - aV0;
		vector3<T> lEdge1 = aV2 - aV0;

		vector3<T> lP = CrossProduct(aRayDir, lEdge1);

		// This is part of the determinant calculation
		T lDet = DotProduct(lEdge0, lP);

		// If lDet is near 0, the ray lies in the plane of the triangle
		if(lDet > -math::type_traits<T>::epsilon && lDet < math::type_traits<T>::epsilon)
			return false;

		T lInvDet = T(1) / lDet;

		vector3<T> lT = aRayOrigin - aV0;

		// Calculate first component of barycentric coordinates, and test bounds
		T u = DotProduct(lT, lP) * lInvDet;
		if(u < T(0) || u > T(1))
			return false;

		vector3f lQ = CrossProduct(lT, lEdge0);

		// Calculate second component of barycentric coordinates, and test bounds
		T v = DotProduct(aRayDir, lQ) * lInvDet;
		if(v < T(0) || (u + v) > T(1))
			return false;

		return true;
	}

}
