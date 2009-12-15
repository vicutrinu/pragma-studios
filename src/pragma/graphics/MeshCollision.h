#pragma once

#include <pragma/graphics/Mesh.h>

namespace pragma
{
	class MeshCollision
	{
	public:
		MeshCollision(const Mesh& aMesh);

		bool IntersectRay( const vector3f& aOrigin, const vector3f& aDirection, float aRayLength, int& aIndex, vector2f& aBarycentric, float& aDistance);
		bool IntersectRay( const vector3f& aOrigin, const vector3f& aDestination);

	private:
		const Mesh& mMesh;
	};
}
