#include "Raytracer.h"
#include <pragma/math/math.h>
#include <pragma/geometry/functions.h>
#include <pragma/graphics/Material.h>

#include <pragma/graphics/Camera.h>
#include <pragma/image/types.h>
#include <pragma/image/Image.h>
#include <pragma/image/functions.h>
#include <pragma/system/system.h>
#include "Samplers.h"
#include "functions.h"
#include <pragma/math/type_traits.h>

#include <time.h>

#include <stdlib.h>

namespace pragma
{
	Raytracer::Raytracer( const Mesh& aMesh, const Point& aLight, const MaterialLibrary& aMaterialLibrary
						, size_t aSamplesPerPixel, size_t aBounces, size_t aRaysPerBounce )
		: mMesh(aMesh)
		, mCollisionMap(mMesh)
		, mLigth(aLight)
		, mMaterialLibrary(aMaterialLibrary)
		, mSamplesPerPixel(aSamplesPerPixel)
		, mMaxDepth(aBounces)
		, mRaysPerBounce(aRaysPerBounce)
	{
		mCollisionMap.ResetStats();			
	}

	/**
	 *	Renderiza la escena utilizando una camara
	 *	Este es el punto de entrada del algoritmo de raytracing. Por cada pixel de la imagen, y utilizando la 
	 *	informacion de la camara, se trazan una serie de rayos para determinar la cantidad de luz que incide sobre
	 *	ese pixel de la imagen.
	 */
	void Raytracer::Render( const Camera& aCamera, Image& aOut )
	{
		BasicSampler lSampler(mSamplesPerPixel);

		RGBPixel* lIter = aOut.begin();

		size_t lImageWidth = aOut.GetWidth();
		size_t lImageHeight = aOut.GetHeight();

		Image lDebugImage(lImageWidth, lImageHeight);
		RGBPixel* lDebugIter = lDebugImage.begin();

		// Get camera inverse tranform (from 2D to 3D world)
		Camera::Transform lTransform = Inverse( aCamera.GetProjection() * aCamera.GetView() );

		int lProgess = 0;

		// Scanline renderer
		for(size_t i = 0; i < lImageHeight; ++i)
		{
			for(size_t j = 0; j < lImageWidth; ++j)
			{
				*lIter = Color(0,0,0);
				// Multiple samples per pixel
				for(size_t s = 0; s < lSampler.GetSamplesPerPixel(); ++s)
				{
					vector2f lOffset = lSampler.GetSample(s);
					vector4<Real> lRay(-1+(((lOffset.x+j)*Real(2)) / (lImageWidth-1))
									  , 1-(((lOffset.y+i)*Real(2)) / (lImageHeight-1))
									  , 1, 1 );
					vector4<Real> lRes = TransformPoint(lTransform, lRay);
					lRes = lRes * (1.f / lRes.w);

					Vector lDir(lRes.x, lRes.y, lRes.z);
					lDir = Normalize( lDir - aCamera.GetPosition() );

					CollisionInfo lInfo;
					if( TraceRay( aCamera.GetPosition(), lDir, Length(lDir * aCamera.GetFarPlane() / lDir.z), lInfo ) )
					{
						*lIter = *lIter + RGBPixel( Shade(lInfo.mPoint, -lDir, lInfo.mNormal, mMaterialLibrary.GetMaterial(lInfo.mMaterial), mRaysPerBounce, 0) );
						*lDebugIter = RGBPixel(mDebug);
					}
				}
				*lIter = *lIter / lSampler.GetSamplesPerPixel();

				++lIter;
				++lDebugIter;

				int lNewProgress = (i * lImageHeight + j) * 100 / (lImageWidth * lImageHeight);
				if(lProgess != lNewProgress)
				{
					printf("%3d%%\b\b\b\b\n", lNewProgress);
					lProgess = lNewProgress;
				}
			}
		}

		ExportToTGA(lDebugImage, "debug.tga");
	}

	Color Raytracer::Shade(const Point& aPoint, const Vector& aDirection, const Vector& aNormal, const Material& aMaterial, size_t aRaysPerBounce, size_t aDepth)
	{
		Color lDirect = DirectIllumination(aPoint, aNormal, aMaterial);
		if(aDepth == mMaxDepth)
			return lDirect;
		else
			return lDirect + IndirectIllumination(aPoint, aDirection, aNormal, aMaterial, aRaysPerBounce, aDepth + 1);
	}

	Color Raytracer::DirectIllumination(const Point& aPoint, const Vector& aNormal, const Material& aMaterial)
	{
		Vector lToLigth = Normalize(mLigth - aPoint);
		Vector lCollisionPoint = aPoint + lToLigth * (4.f * math::type_traits<Real>::epsilon);
		
		if(aMaterial.GetType() == Material::eSolid)
		{
			bool lShadowRayCollision = mCollisionMap.IntersectRay( lCollisionPoint, mLigth );
			if( !lShadowRayCollision ) 
			{
				// Direct Lighting
				Real lDiffuse = DotProduct(aNormal, Normalize(mLigth-lCollisionPoint));
				if(lDiffuse > 0)
					return aMaterial.GetdiffuseColor() * lDiffuse;// / math::type_traits<Real>::Pi;
			}
		}

		return Color(0,0,0);
	}

	Color Raytracer::IndirectIllumination( const Point& aPoint, const Vector& aDirection, const Vector& aNormal, const Material& aMaterial, size_t aRaysPerBounce, size_t aDepth )
	{
		// Indirect Lighting
		Color lIndirect(0,0,0);
		Real lBRDF = Real(1) / math::type_traits<Real>::Pi;
		Real lPDF = math::type_traits<Real>::DoublePi;
		if(aRaysPerBounce > 0)
		{
			/*matrix3x3<Real> lMat;
			CreateLocalSpace(lMat, aNormal);*/

			for(size_t i = 0; i < aRaysPerBounce; ++i)
			{
				Real u1 = Random<Real>();
				Real u2 = Random<Real>();

				Vector lDir = UniformSampleHemisphere(u1, u2, aNormal);
				//lDir = TransformPoint(lMat, lDir);

				mDebug = lDir;
				Vector lPoint = aPoint + lDir * (4.f * math::type_traits<Real>::epsilon);
				
				CollisionInfo lInfo;
				if( TraceRay(lPoint, lDir, 1000, lInfo) )
				{
					Real lCos = DotProduct( aNormal, lDir );
					lIndirect+= ( aMaterial.GetdiffuseColor() 
								* Shade( lInfo.mPoint, -lDir, lInfo.mNormal, mMaterialLibrary.GetMaterial(lInfo.mMaterial), 1, aDepth ))
								//* lBRDF * lPDF
								* lCos;
				}
			}
			
			lIndirect = lIndirect / mRaysPerBounce;
		}

		return lIndirect;
	}

	bool Raytracer::TraceRay(const Point& aPosition, const Vector& aDirection, Real aLength, CollisionInfo& aInfo)
	{
		MeshCollision::TResult lResult;

		if( mCollisionMap.IntersectRay( aPosition, aDirection, aLength, lResult ) )
		{
			// Collision, shade point
			size_t lCount;
			const Mesh::TTriangle& lTri = mMesh.GetTriangles(lCount)[lResult.mTriangleIndex];

			aInfo.mPoint = lResult.mPosition;
			ComputeFromBarycentric( mMesh.GetVertexNormal(lTri.mVertexNormal[0])
								  , mMesh.GetVertexNormal(lTri.mVertexNormal[1])
								  , mMesh.GetVertexNormal(lTri.mVertexNormal[2])
								  , lResult.mBarycentric
								  , aInfo.mNormal );

			aInfo.mMaterial = lTri.mMaterial;
			return true;
		}
		else
			return false;
	}

}
