#pragma once

#include "vector2.h"

namespace pragma
{
	template <typename T>
	struct base_vector3
	{
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
	
	template <typename T>
	struct vector3 : public base_vector3<T>
	{
		vector3 () { }
		
		vector3 (const base_vector3<T>& aVector)
		{
			*this = aVector;
		}

		vector3 (const T& aX, const T& aY, const T& aZ)
		{
			base_vector3<T>::x = aX;
			base_vector3<T>::y = aY;
			base_vector3<T>::z = aZ;
		}

		vector3 (const vector2<T>& aVector2, const T& aValue)
		{
			base_vector3<T>::x = aVector2.x;
			base_vector3<T>::y = aVector2.y;
			base_vector3<T>::z = aValue;
		}

	};
};