#pragma once

#include <pragma/graphics/MeshCollision.h>
#include <pragma/graphics/types.h>

namespace pragma
{
	class MaterialLibrary;
	class Material;
	class Mesh;
	class Camera;
	class Image;

	class Raytracer
	{
	public:
				Raytracer		( const Mesh& aMesh, const Point& aLight
								, const MaterialLibrary& aMaterialLibrary
								, size_t aSamplesPerPixel, size_t aBounces, size_t aRaysPerBounce );

		void	Render			( const Camera& aCamera, Image& aOut );

		size_t	TracedRays		() const { return mCollisionMap.TracedRays(); }

	private:
		struct CollisionInfo
		{
			Point		mPoint;
			Vector		mNormal;
			uint32		mMaterial;
		};

		Color	Shade				( const Point& aPoint, const Vector& aDirection, const Vector& aNormal, const Material& aMaterial, size_t aRaysPerBounce, size_t aDepth );
		Color	DirectIllumination	( const Point& aPoint, const Vector& aNormal, const Material& aMaterial );
		Color	IndirectIllumination( const Point& aPoint, const Vector& aDirection, const Vector& aNormal, const Material& aMaterial, size_t aRaysPerBounce, size_t aDepth );
		bool	TraceRay			( const Point& aPosition, const Vector& aDirection, Real aLength, CollisionInfo& aInfo );

	private:
		const Mesh&				mMesh;
		const MaterialLibrary&	mMaterialLibrary;
		MeshCollision			mCollisionMap;
		Point					mLigth;

		size_t					mSamplesPerPixel;
		size_t					mMaxDepth;
		size_t					mRaysPerBounce; // First hit only

		Vector					mDebug;
	};
}
