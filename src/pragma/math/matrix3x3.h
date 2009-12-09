#pragma once

#include "vector3.h"

namespace pragma
{

	template <typename T>
	struct matrix3x3
	{
		matrix3x3 () { }

		matrix3x3 ( const vector3<T>& aRow0, const T& aRow1, const T& aRow2 )
				  : mRow0(aRow0), mRow1(aRow1), mRow2(aRow1) { }

		matrix3x3 ( T a00, T a01, T a02
				  , T a10, T a11, T a12
				  , T a20, T a21, T a22 )
		{
			i[0][0] = a00; i[0][1] = a01; i[0][2] = a02;
			i[1][0] = a10; i[1][1] = a11; i[1][2] = a12;
			i[2][0] = a20; i[2][1] = a21; i[2][2] = a22;
		}

		union
		{
			T i[3][3];
			struct
			{
				vector3<T> mRow0;
				vector3<T> mRow1;
				vector3<T> mRow2;
			};
		};
	};

}
