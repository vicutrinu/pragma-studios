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
	T SqLength(const vector3<T>& aVector)
	{
		return DotProduct(aVector, aVector);
	}

	template <typename T>
	T Length(const vector3<T>& aVector)
	{
		return pragma::sqrt<T>( SqLength(aVector) );
	}

	template <typename T>
	vector3<T> Normalize(const vector3<T>& aVector)
	{
		return aVector * ( T(1) / Length(aVector) );
	}
}
