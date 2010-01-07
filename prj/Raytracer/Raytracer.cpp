#include "Raytracer.h"
#include <pragma/math/math.h>
#include <pragma/geometry/functions.h>
#include <pragma/graphics/Material.h>

#include <time.h>

namespace pragma
{
	template <typename T>
	inline T Random()
	{
		return T(T(rand()) / RAND_MAX);
	}

	template <typename T>
	inline T Random(T aMin, T aMax)
	{
		T lVal = T(T(rand()) / RAND_MAX);
		return (lVal * (aMax - aMin)) + aMin;
	}

	Vector SampleHemisphere(const Vector& aNormal)
	{
		Real u1 = Random<Real>();
		Real u2 = Random<Real>();

		const Real r = sqrt<Real>(u1);  
		const Real theta = math::type_traits<Real>::DoublePi * u2;  
	  
		const Real x = r * Cos(theta);  
		const Real z = r * Sin(theta);  
	  
		Vector lRetVal(x, sqrt<Real>(max<Real>(0.0f, 1 - x * x - z * z)), z);

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

		matrix3x3<Real> lMat( lX.x, lY.x, lZ.x
							, lX.y, lY.y, lZ.y
							, lX.z, lY.z, lZ.z );

		lRetVal = TransformPoint(lMat, lRetVal);
				
		return lRetVal;
	}

	Raytracer::Raytracer( const Mesh& aMesh, const Point& aLight, const MaterialLibrary& aMaterialLibrary
						, size_t aBounces, size_t aRaysPerBounce )
		: mMesh(aMesh)
		, mCollisionMap(mMesh)
		, mLigth(aLight)
		, mMaterialLibrary(aMaterialLibrary)
		, mBounces(aBounces)
		, mRaysPerBounce(aRaysPerBounce)
	{
		srand(clock());
		mCollisionMap.ResetStats();			
	}

	Color Raytracer::Shade(const Point& aPoint, const Vector& aNormal, const Material& aMaterial, size_t aDepth)
	{
		Color lRetVal(0,0,0);

		Vector lToLigth = Normalize(mLigth - aPoint);
		Vector lCollisionPoint = aPoint + lToLigth * (4.f * math::type_traits<Real>::epsilon);
		
		bool lShadowRayCollision = mCollisionMap.IntersectRay( lCollisionPoint, mLigth );
		if( !lShadowRayCollision ) 
		{
			// Direct Lighting
			Real lDiffuse = DotProduct(aNormal, Normalize(mLigth-lCollisionPoint));
			if(lDiffuse > 0)
			{
				lRetVal = lRetVal + aMaterial.GetdiffuseColor() * lDiffuse;
			}
		}

		if(aDepth == 0)
			return lRetVal;
		aDepth--;

		if(mRaysPerBounce > 0)
		{
			// Indirect Lighting
			Color lIndirect(0,0,0);

			for(size_t i = 0; i < mRaysPerBounce; ++i)
			{
				Vector lDir = SampleHemisphere(aNormal);
				mDebug = lDir;
				Vector lPoint = aPoint + lDir * (4.f * math::type_traits<Real>::epsilon);
				lIndirect = lIndirect + (aMaterial.GetdiffuseColor() * ( TraceRay(lPoint, lDir, 1000, aDepth) * DotProduct( aNormal, lDir)));
			}
			
			lRetVal = lRetVal + ( lIndirect / mRaysPerBounce );
		}

		return lRetVal;
	}

	Color Raytracer::TraceCameraRay(const Point& aPosition, const Vector& aDirection, Real aLength)
	{
		return TraceRay(aPosition, aDirection, aLength, mBounces);
	}

	Color Raytracer::TraceRay(const Point& aPosition, const Vector& aDirection, Real aLength, size_t aDepth)
	{
		Color lRetVal(0,0,0);

		MeshCollision::TResult lResult;

		if( mCollisionMap.IntersectRay( aPosition, aDirection, aLength, lResult ) )
		{
			// Collision, shade point
			size_t lCount;
			const Mesh::TTriangle& lTri = mMesh.GetTriangles(lCount)[lResult.mTriangleIndex];

			Vector lNormal;
			ComputeFromBarycentric( mMesh.GetVertexNormal(lTri.mVertexNormal[0])
				, mMesh.GetVertexNormal(lTri.mVertexNormal[1])
				, mMesh.GetVertexNormal(lTri.mVertexNormal[2])
				, lResult.mBarycentric
				, lNormal );
			lRetVal = lRetVal + Shade(lResult.mPosition, lNormal, mMaterialLibrary.GetMaterial(lTri.mMaterial), aDepth);
		}

		return lRetVal;
	}

}
