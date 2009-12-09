#pragma once

#include "vector2.h"

namespace pragma
{
	template <typename T>
	struct vector3
	{
		vector3 () { }

		vector3 (const T& aX, const T& aY, const T& aZ)
			: x(aX), y(aY), z(aZ) { }

		vector3 (const vector2<T>& aVector2, const T& aValue)
			: x(aVector2.x), y(aVector2.y), z(aValue) { }

		union
		{
			T i[3];
			struct
			{
				T x;
				T y;
				T z;
			};
		};
	};
};