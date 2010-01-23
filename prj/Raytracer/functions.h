#pragma once

#include <pragma/math/types.h>
#include <pragma/graphics/types.h>

namespace pragma
{
	inline void CreateLocalSpace(matrix3x3<Real>& aOut, const Vector& aNormal)
	{
		Vector lZ;
		Vector lY = Normalize(aNormal);
		if(abs(aNormal.x) > abs(aNormal.y))
		{
			Real lInvLen = Real(1) / sqrt<Real>( lY.x * lY.x + lY.z * lY.z);
			lZ = Vector( -lY.z * lInvLen, 0, lY.x * lInvLen );
		}
		else
		{
			Real lInvLen = Real(1) / sqrt<Real>( lY.y * lY.y + lY.z * lY.z);
			lZ = Vector( 0, lY.z * lInvLen, -lY.y * lInvLen );			
		}
		Vector lX = CrossProduct(lY, lZ);

		aOut = matrix3x3<Real>( lX.x, lY.x, lZ.x
							  , lX.y, lY.y, lZ.y
							  , lX.z, lY.z, lZ.z );
	}
}
