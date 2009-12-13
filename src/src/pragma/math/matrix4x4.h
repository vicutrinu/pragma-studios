#pragma once

#include "vector4.h"

namespace pragma
{

	template <typename T>
	struct matrix4x4
	{
		matrix4x4 () { }

		matrix4x4 ( const vector4<T>& aRow0, const vector4<T>& aRow1, const vector4<T>& aRow2, const vector4<T>& aRow3 )
		{
			i[0][0] = aRow0.x; i[0][1] = aRow0.y; i[0][2] = aRow0.z; i[0][3] = aRow0.w;
			i[1][0] = aRow1.x; i[1][1] = aRow1.y; i[1][2] = aRow1.z; i[1][3] = aRow1.w;
			i[2][0] = aRow2.x; i[2][1] = aRow2.y; i[2][2] = aRow2.z; i[2][3] = aRow2.w;
			i[3][0] = aRow3.x; i[3][1] = aRow3.y; i[3][2] = aRow3.z; i[3][3] = aRow3.w;
		}

		matrix4x4 ( T a00, T a01, T a02, T a03
				  , T a10, T a11, T a12, T a13
				  , T a20, T a21, T a22, T a23
				  , T a30, T a31, T a32, T a33 )
		{
			i[0][0] = a00; i[0][1] = a01; i[0][2] = a02; i[0][3] = a03;
			i[1][0] = a10; i[1][1] = a11; i[1][2] = a12; i[1][3] = a13;
			i[2][0] = a20; i[2][1] = a21; i[2][2] = a22; i[2][3] = a23;
			i[3][0] = a30; i[3][1] = a31; i[3][2] = a32; i[3][3] = a33;
		}

		union 
		{
			T i[4][4];
			struct 
			{
				base_vector4<T> mRow0;
				base_vector4<T> mRow1;
				base_vector4<T> mRow2;
				base_vector4<T> mRow3;
			};
		};
	};

}
