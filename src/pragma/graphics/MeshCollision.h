#pragma once

#include <pragma/graphics/Mesh.h>

namespace pragma
{
	class MeshCollision
	{
	public:
		MeshCollision(const Mesh& aMesh);

		bool IntersectRay( const Point& aOrigin, const Vector& aDirection, Real aRayLength, int& aIndex, Vector2& aBarycentric, Real& aDistance );
		bool IntersectRay( const Point& aOrigin, const Point& aDestination );

	private:
		const Mesh& mMesh;
	};
}
