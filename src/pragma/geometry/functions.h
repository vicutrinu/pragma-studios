#pragma once

#include "pragma/geometry/types.h"
#include "pragma/math/math.h"
#include "pragma/math/functions.h"

namespace pragma
{
	inline bool PointInsideAABB(const AABB& aBox, const point3& aPoint)
	{
		if(	aBox.mMin.x <= aPoint.x &&
			aBox.mMin.y <= aPoint.y &&
			aBox.mMin.z <= aPoint.z &&
			aBox.mMax.x >= aPoint.x &&
			aBox.mMax.y >= aPoint.y &&
			aBox.mMax.z >= aPoint.z )
			return true;
		else
			return false;
	}

	template <typename T>
	T SqLength(const base_vector3<T>& aVector)
	{
		return DotProduct(aVector, aVector);
	}

	template <typename T>
	T Length(const base_vector3<T>& aVector)
	{
		return pragma::sqrt<T>( SqLength(aVector) );
	}

	template <typename T>
	vector3<T> Normalize(const base_vector3<T>& aVector)
	{
		return aVector * ( T(1) / Length(aVector) );
	}

	template< typename T>
	void ComputeFromBarycentric( const T& aA, const T& aB, const T& aC
							   , const Vector2& aBarycentric
							   , T& aResult )
	{
		aResult = aA * Real(1 - aBarycentric.x - aBarycentric.y) + 
				  aB * Real(aBarycentric.x) + 
				  aC * Real(aBarycentric.y);
	}
}
