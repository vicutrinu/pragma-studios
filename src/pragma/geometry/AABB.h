#pragma once

#include <pragma/math/math.h>

namespace pragma
{

	struct AABB
	{
		AABB() { }
		AABB( const point3& aMin, const point3& aMax) : mMin(aMin), mMax(aMax) { }

		void AddPoint(const point3& aPoint)
		{
			mMin.x = min(aPoint.x, mMin.x);
			mMin.y = min(aPoint.y, mMin.y);
			mMin.z = min(aPoint.z, mMin.z);

			mMax.x = max(aPoint.x, mMax.x);
			mMax.y = max(aPoint.y, mMax.y);
			mMax.z = max(aPoint.z, mMax.z);
		}

		point3 mMin;
		point3 mMax;
	};

}
