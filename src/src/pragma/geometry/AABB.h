#pragma once

namespace pragma
{

	struct AABB
	{
		AABB( const point3& aMin, const point3& aMax) : mMin(aMin), mMax(aMax) { }

		point3 mMin;
		point3 mMax;
	};

}
