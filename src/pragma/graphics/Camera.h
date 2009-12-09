#pragma once

#include <pragma/graphics/types.h>

namespace pragma
{

	class Camera
	{
	public:
		Camera();
		
		void SetTransform(const Point& aPosition, const Vector& aDirection, const Vector& aUp);
		void SetProjection(Real aFOV, Real aAspectRatio, Real aNearPlane, Real aFarPlane);

		const matrix4x4f GetTransform() const { return mTransform; }
		const matrix4x4f GetProjection() const { return mProjection; }
	private:
		matrix4x4f mProjection;
		matrix4x4f mTransform;
	};

}
