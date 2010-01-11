#pragma once

#include <pragma/math/types.h>
#include <pragma/graphics/types.h>

namespace pragma
{
	inline void CreateLocalSpace(matrix3x3<Real>& aOut, const Vector& aNormal)
	{
		Vector lX;
		Vector lY = Normalize(aNormal);
		if(abs(aNormal.x) > abs(aNormal.y))
		{
			if(abs(aNormal.y) > abs(aNormal.z))
			{
				lX.x = lY.y;
				lX.y = lY.x;
				lX.z = lY.z;
			}
			else
			{
				lX.x = lY.z;
				lX.y = lY.y;
				lX.z = lY.x;
			}
		}
		else
		{
			if(abs(aNormal.x) > abs(aNormal.z))
			{
				lX.x = lY.y;
				lX.y = lY.x;
				lX.z = lY.z;
			}
			else
			{
				lX.x = lY.x;
				lX.y = lY.z;
				lX.z = lY.y;
			}
		}
		Vector lZ = CrossProduct(lX, lY);

		aOut = matrix3x3<Real>( lX.x, lY.x, lZ.x
							  , lX.y, lY.y, lZ.y
							  , lX.z, lY.z, lZ.z );
	}
}
