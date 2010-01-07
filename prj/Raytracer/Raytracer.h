#pragma once

#include <pragma/graphics/MeshCollision.h>
#include <pragma/graphics/types.h>

namespace pragma
{
	class MaterialLibrary;
	class Material;
	class Mesh;

	class Raytracer
	{
	public:
				Raytracer		( const Mesh& aMesh, const Point& aLight
								, const MaterialLibrary& aMaterialLibrary
								, size_t aBounces, size_t aRaysPerBounce );

		Color	Shade			( const Point& aPoint, const Vector& aNormal, const Material& aMaterial, size_t aDepth );
		Color	TraceCameraRay	( const Point& aPosition, const Vector& aDirection, Real aLength );
		Color	TraceRay		( const Point& aPosition, const Vector& aDirection, Real aLength, size_t aDepth );

		size_t	TracedRays		() const { return mCollisionMap.TracedRays(); }

		const Vector& Debug		() const { return mDebug; }

	private:
		const Mesh&				mMesh;
		const MaterialLibrary&	mMaterialLibrary;
		MeshCollision			mCollisionMap;
		Point					mLigth;

		size_t					mBounces;
		size_t					mRaysPerBounce;

		Vector					mDebug;
	};
}
