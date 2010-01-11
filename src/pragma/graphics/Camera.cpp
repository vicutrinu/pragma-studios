#include "Camera.h"
#include <pragma/geometry/functions.h>

namespace pragma
{
	void Camera::SetTransform(const Point& aPosition, const Vector& aLookAt, const Vector& aUp)
	{
		mPosition = aPosition;
		mLookAt	  = aLookAt;
		mUp		  = aUp;

		Vector lZ = Normalize(aPosition - aLookAt);
		Vector lX = Normalize(CrossProduct(aUp, lZ));
		Vector lY = Normalize(CrossProduct(lZ, lX));

		Transform lRot( lX.x, lX.y, lX.z, 0
					  , lY.x, lY.y, lY.z, 0
					  , lZ.x, lZ.y, lZ.z, 0
					  ,    0,    0,    0, 1 );

		Transform lTrans(    1,    0,    0, -aPosition.x
						,    0,    1,    0, -aPosition.y
						,    0,    0,    1, -aPosition.z
						,    0,    0,    0, 1 );

		mView = lRot * lTrans;
		/*
		mTransform.mRow0 = vector4f(lX.x, lX.y, lX.z, DotProduct(-aPosition, lX));//vector3f(lX.x, lY.x, lZ.x)));
		mTransform.mRow1 = vector4f(lY.x, lY.y, lY.z, DotProduct(-aPosition, lY));//vector3f(lX.y, lY.y, lZ.y)));
		mTransform.mRow2 = vector4f(lZ.x, lZ.y, lZ.z, DotProduct(-aPosition, lZ));//vector3f(lX.z, lY.z, lZ.z)));
		mTransform.mRow3 = vector4f(   0,    0,    0, 1);
		*/
	}

	void Camera::SetProjection(Real aFOV, Real aAspectRatio, Real aNearPlane, Real aFarPlane)
	{
		mFov = aFOV;
		mAspectRatio = aAspectRatio;
		mNearPlane = aNearPlane;
		mFarPlane = aFarPlane;

		Real A = Cot(aFOV / 2) / aAspectRatio;
		Real B = Cot(aFOV / 2);
		Real C = -(aFarPlane + aNearPlane) / (aFarPlane - aNearPlane);
		Real D = -(2 * aFarPlane * aNearPlane) / (aFarPlane - aNearPlane);
		Real E = -1;

		mProjection = Transform( A, 0, 0, 0
							   , 0, B, 0, 0
							   , 0, 0, C, D
							   , 0, 0, E, 0 );
	}
}
