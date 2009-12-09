#pragma once

namespace pragma
{

	template <typename T>
	struct vector2
	{
		vector2() { }

		vector2 (const T& aX, const T& aY)
			: x(aX), y(aY) { }

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

};