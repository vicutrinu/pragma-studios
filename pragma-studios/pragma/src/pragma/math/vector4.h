#pragma once

#include "vector2.h"

namespace pragma
{
	template <typename T>
	struct vector4
	{
		vector4 () { }

		vector4 (const T& aX, const T& aY, const T& aZ, const T& aW)
			: x(aX), y(aY), z(aZ), w(aW) { }

		vector4 (const vector3<T>& aVector3, const T& aValue)
			: x(aVector3.x), y(aVector3.y), z(aVector3.z), w(aValue) { }

		union
		{
			T i[4];
			struct
			{
				T x;
				T y;
				T z;
				T w;
			};
		};
	};
};