#pragma once

namespace pragma
{
	
	template <typename T>
	struct base_vector2
	{
		union 
		{
			T i[2];
			struct
			{
				T x;
				T y;
			};
		};
	};

	template <typename T>
	struct vector2 : public base_vector2<T>
	{
		vector2() { }
		
		vector2(const base_vector2<T>& aVector)
		{
			*(base_vector2*)this = aVector;
		}

		vector2 (const T& aX, const T& aY)
		{
			base_vector2<T>::x = aX;
			base_vector2<T>::y = aY;
		}
	};

};