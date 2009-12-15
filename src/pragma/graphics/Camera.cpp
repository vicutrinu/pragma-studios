#include "Camera.h"
#include <pragma/geometry/functions.h>

namespace pragma
{
	Camera::Camera()
	{
	}
	
	void Camera::SetTransform(const Point& aPosition, const Vector& aLookAt, const Vector& aUp)
	{
		vector3f lZ = Normalize<float>(aLookAt - aPosition);
		vector3f lX = Normalize<float>(CrossProduct(aUp, lZ));
		vector3f lY = Normalize<float>(CrossProduct(lZ, lX));
		mTransform.mRow0 = vector4f(lX.x, lX.y, lX.z, DotProduct(-aPosition, lX));//vector3f(lX.x, lY.x, lZ.x)));
		mTransform.mRow1 = vector4f(lY.x, lY.y, lY.z, DotProduct(-aPosition, lY));//vector3f(lX.y, lY.y, lZ.y)));
		mTransform.mRow2 = vector4f(lZ.x, lZ.y, lZ.z, DotProduct(-aPosition, lZ));//vector3f(lX.z, lY.z, lZ.z)));
		mTransform.mRow3 = vector4f(   0,    0,    0, 1);
	}

	void Camera::SetProjection(Real aFOV, Real aAspectRatio, Real aNearPlane, Real aFarPlane)
	{
		Real A = Cot(aFOV / 2) / aAspectRatio;
		Real B = Cot(aFOV / 2);
		Real C = -(aFarPlane + aNearPlane) / (aFarPlane - aNearPlane);
		Real D = -(2 * aFarPlane * aNearPlane) / (aFarPlane - aNearPlane);
		Real E = -1;

		mProjection.mRow0 = vector4f(A, 0, 0, 0);
		mProjection.mRow1 = vector4f(0, B, 0, 0);
		mProjection.mRow2 = vector4f(0, 0, C, D);
		mProjection.mRow3 = vector4f(0, 0, E, 0);
	}
}
