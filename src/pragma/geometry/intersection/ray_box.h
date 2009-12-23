/**
 *	@brief	Calculos de las intersecciones entre un rayo y una caja.
 */
#pragma once

#include "pragma/math/functions.h"
#include "pragma/math/type_traits.h"
#include "pragma/geometry/functions.h"

namespace pragma
{

	template <typename T>
	bool IntersectRayBox(const vector3<T>& aMin, const vector3<T>& aMax, const vector3<T>& aOrigin, const vector3<T>& aDirection, vector3<T>& aIntersectionPoint)
	{
		bool Inside = true;
		vector3<T> MaxT(-1, -1, -1);

		// Find candidate planes.
		for(uint32 i = 0; i < 3; ++i)
		{
			if(aOrigin.i[i] < aMin.i[i])
			{
				aIntersectionPoint.i[i]	= aMin.i[i];
				Inside		= false;

				// Calculate T distances to candidate planes
				if( aDirection.i[i] != 0 )
					MaxT.i[i] = (aMin.i[i] - aOrigin.i[i]) / aDirection.i[i];
			}
			else if(aOrigin.i[i] > aMax.i[i])
			{
				aIntersectionPoint.i[i]	= aMax.i[i];
				Inside		= false;

				// Calculate T distances to candidate planes
				if( aDirection.i[i] != 0)
					MaxT.i[i] = (aMax.i[i] - aOrigin.i[i]) / aDirection.i[i];
			}
		}

		// Ray origin inside bounding box
		if(Inside)
		{
			aIntersectionPoint = aOrigin;
			return true;
		}

		// Get largest of the maxT's for final choice of intersection
		uint32 WhichPlane = 0;
		if(MaxT.i[1] > MaxT.i[WhichPlane])
			WhichPlane = 1;
		if(MaxT.i[2] > MaxT.i[WhichPlane])
			WhichPlane = 2;

		// Check final candidate actually inside box
		if( MaxT.i[WhichPlane] < 0)
			return false;

		for(int i = 0; i < 3; ++i)
		{
			if(i != WhichPlane)
			{
				aIntersectionPoint.i[i] = aOrigin.i[i] + MaxT.i[WhichPlane] * aDirection.i[i];

				if( aIntersectionPoint.i[i] < aMin.i[i] - math::type_traits<float>::epsilon ||
					aIntersectionPoint.i[i] > aMax.i[i] + math::type_traits<float>::epsilon )
					return false;
				/* version sin epsilon
				if(aIntersectionPoint.i[i] < aMin.i[i] || aIntersectionPoint.i[i] > aMax.i[i])
					return false;
				*/
			}
		}
		return true;	// ray hits box
	}

	template <typename T>
	bool IntersectRayBox(const vector3<T>& aMin, const vector3<T>& aMax, const vector3<T>& aOrigin, const vector3<T>& aDirection, T aLength, vector3<T>& aIntersectionPoint)
	{
		if( IntersectRayBox(aMin, aMax, aOrigin, aDirection, aIntersectionPoint) &&
			(SqLength(aIntersectionPoint - aOrigin) <= (aLength*aLength)) )
		{
			return true;			
		}
		return false;
	}

	template<typename T>
	bool IntersectRayBox( const base_vector3<T>& aCenter, const base_vector3<T>& aSize
						, const base_vector3<T>& aRayOrigin, const base_vector3<T>& aRayDir )
	{
		base_vector3<T> BoxExtents, Diff;

		Diff.x = aRayOrigin.x - aCenter.x;
		BoxExtents.x = aSize.x;
		if( (abs(Diff.x) > BoxExtents.x) &&
			(Diff.x * aRayDir.x >= 0.0f) )
			return false;

		Diff.y = aRayOrigin.y - aCenter.y;
		BoxExtents.y = aSize.y;
		if(abs(Diff.y) > BoxExtents.y && Diff.y * aRayDir.y >= 0.0f)
			return false;

		Diff.z = aRayOrigin.z - aCenter.z;
		BoxExtents.z = aSize.z;
		if(abs(Diff.z) > BoxExtents.z && Diff.z * aRayDir.z >= 0.0f)
			return false;

		T fAWdU[3];
		fAWdU[0] = abs<T>(aRayDir.x);
		fAWdU[1] = abs<T>(aRayDir.y);
		fAWdU[2] = abs<T>(aRayDir.z);

		T f;
		f = aRayDir.y * Diff.z - aRayDir.z * Diff.y;
		if(abs<T>(f)>BoxExtents.y*fAWdU[2] + BoxExtents.z*fAWdU[1])
			return false;
		f = aRayDir.z * Diff.x - aRayDir.x * Diff.z;
		if(abs<T>(f)>BoxExtents.x*fAWdU[2] + BoxExtents.z*fAWdU[0])
			return false;
		f = aRayDir.x * Diff.y - aRayDir.y * Diff.x;
		if(abs<T>(f)>BoxExtents.x*fAWdU[1] + BoxExtents.y*fAWdU[0])
			return false;

		return true;
	}

	template <typename T>
	bool IntersectRayBox(const AABB& aBB, const base_vector3<T>& aRayOrigin, const base_vector3<T>& aRayDir )
	{
		Vector lSize = (aBB.mMax - aBB.mMin);
		lSize = lSize / 2;
		Point lCenter = Point(aBB.mMin) + lSize;
		return IntersectRayBox(lCenter, lSize, aRayOrigin, aRayDir);
	}

}
