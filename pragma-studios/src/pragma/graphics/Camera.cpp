#include "Camera.h"
#include <pragma/geometry/functions.h>

namespace pragma
{
	Camera::Camera()
	{
	}
	
	void Camera::SetTransform(const Point& aPosition, const Vector& aDirection, const Vector& aUp)
	{
		vector3f lZ = Normalize(aDirection);
		vector3f lY = Normalize(aUp);
		vector3f lX = CrossProduct(lY, lZ);
		mTransform.mRow0 = vector4f(lX.x, lY.x, lZ.x, 0);
		mTransform.mRow1 = vector4f(lX.y, lY.y, lZ.y, 0);
		mTransform.mRow2 = vector4f(lX.z, lY.z, lZ.z, 0);
		mTransform.mRow3 = vector4f(-aPosition.z, -aPosition.y, -aPosition.z, 1);
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
