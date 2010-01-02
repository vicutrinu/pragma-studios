#include <pragma/graphics/Mesh.h>
#include <pragma/graphics/import/Parsers.h>
#include <pragma/graphics/Camera.h>
#include <pragma/graphics/MeshCollision.h>

#include <pragma/math/types.h>
#include <pragma/math/functions.h>

#include <pragma/geometry/functions.h>

#include <stdio.h>

#include <pragma/system/clock.h>
#include <pragma/graphics/Material.h>
#include <pragma/image/Image.h>
#include <pragma/image/functions.h>

using namespace pragma;

#define SAMPLES_PER_PIXEL	4
#define IMAGE_SIZE			512
#define INDIRECT_DEPTH		2
#define INDIRECT_RAYS		100

Point lLigth(0,54,0);

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

	Vector RandomVector(const Vector& aNormal)
	{
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

		Vector lRetVal = Normalize( lX * Random<Real>(-1, 1) +
									aNormal * Random<Real>(0, 2)+
									lZ * Random<Real>(-1,1) );

		return lRetVal;
	}

	class Multisample
	{
	public:
		Multisample(size_t aSamplesPerPixel)
			: mSamplesPerPixel(aSamplesPerPixel)
		{
			mValues.reserve(mSamplesPerPixel);
			size_t lWidth = (size_t)pragma::sqrt<float>((float)mSamplesPerPixel);
			size_t lHeigth = mSamplesPerPixel / lWidth;
			vector2f lOffset( 1.f / lWidth, 1.f / lHeigth);
			vector2f lSample = lOffset / 2;
			for(size_t i = 0; i < lHeigth; ++i)
			{
				for(size_t j = 0; j < lWidth; ++j)
				{
					mValues.push_back(lSample);										
					lSample.x+= lOffset.x;
				}
				lSample.y+= lOffset.y;
			}
		}

		size_t GetSamplesPerPixel() const { return mSamplesPerPixel; }
		const vector2f GetSampleOffset(size_t aIndex) const
		{
			return mValues[aIndex];
			
		}

	private:
		size_t mSamplesPerPixel;
		std::vector<vector2f> mValues;
	};

	class Raytracer
	{
	public:
		Raytracer(const Mesh& aMesh, const MaterialLibrary& aMaterialLibrary)
			: mMesh(aMesh)
			, mCollisionMap(mMesh)
			, mMaterialLibrary(aMaterialLibrary)
		{
			mCollisionMap.ResetStats();			
		}

		Color Shade(const Point& aPoint, const Vector& aNormal, const Material& aMaterial, size_t aDepth)
		{
			Color lRetVal(0,0,0);

			Vector lToLigth = Normalize(lLigth - aPoint);
			Vector lCollisionPoint = aPoint + lToLigth * (4.f * math::type_traits<Real>::epsilon);
			bool lShadowRayCollision = mCollisionMap.IntersectRay( lCollisionPoint, lLigth );
			if( !lShadowRayCollision ) 
			{
				// Direct Lighting
				Real lDiffuse = DotProduct(aNormal, Normalize(lLigth-lCollisionPoint));
				if(lDiffuse > 0)
				{
					lRetVal = lRetVal + aMaterial.GetdiffuseColor() * lDiffuse;
				}
			}

			aDepth--;
			if(aDepth == 0)
				return lRetVal;

#if defined INDIRECT_RAYS && INDIRECT_RAYS > 0
			// Indirect Lighting
			Color lIndirect(0,0,0);

			for(size_t i = 0; i < INDIRECT_RAYS; ++i)
			{
				Vector lDir = RandomVector(aNormal);
				Vector lPoint = aPoint + lDir * (4.f * math::type_traits<Real>::epsilon);
				lIndirect = lIndirect + (aMaterial.GetdiffuseColor() * ( TraceRay(lPoint, lDir, 1000, aDepth) * DotProduct( aNormal, lDir)));
			}
			lRetVal = lRetVal + ( lIndirect / INDIRECT_RAYS );
#endif			
			return lRetVal;
		}

		size_t TracedRays() const
		{
			return mCollisionMap.TracedRays();
		}

		Color TraceCameraRay(const Point& aPosition, const Vector& aDirection, Real aLength)
		{
			return TraceRay(aPosition, aDirection, aLength, INDIRECT_DEPTH);
		}

		Color TraceRay(const Point& aPosition, const Vector& aDirection, Real aLength, size_t aDepth)
		{
			Color lRetVal(0,0,0);

			if(aDepth == 0)
				return lRetVal;

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

	private:
		const Mesh& mMesh;
		const MaterialLibrary& mMaterialLibrary;
		MeshCollision mCollisionMap;
	};
}

int main(int argc, char* argv[])
{
	// Load ASE file
	TMillisecond lStart = GetTimeMilliseconds();
	pragma::Mesh lMesh;
	MaterialLibrary lMaterialLibrary(32);
	ParseOBJ("scenes/Cornell.obj", lMesh, lMaterialLibrary);
	TMillisecond lEnd = GetTimeMilliseconds();
	printf("Parsed file in %d milliseconds\n", lEnd - lStart);

	Vector lCameraPos(0,27.5,75);
	Camera lCamera;
	lCamera.SetProjection(45, 1, 1.f, 300.f);
	lCamera.SetTransform( lCameraPos, Vector(0,27.5,0), Vector(0,1,0) );

	matrix4x4f lTransform = lCamera.GetProjection() * lCamera.GetTransform();
	lTransform = Inverse(lTransform);

	lStart = GetTimeMilliseconds();
	lEnd = GetTimeMilliseconds();
	printf("Created KdTree in %d milliseconds\n", lEnd - lStart);

	Image lImage(IMAGE_SIZE, IMAGE_SIZE);
	RGBPixel* lIter = lImage.begin();

	lStart = GetTimeMilliseconds();

	Multisample lMultisample(SAMPLES_PER_PIXEL);

	Raytracer lRaytracer(lMesh, lMaterialLibrary);

	int lProgess = 0;

	// Scanline renderer
	for(size_t i = 0; i < IMAGE_SIZE; ++i)
	{
		for(size_t j = 0; j < IMAGE_SIZE; ++j)
		{
			*lIter = Color(0,0,0);
			// Multiple samples per pixel
			for(size_t s = 0; s < lMultisample.GetSamplesPerPixel(); ++s)
			{
				vector2f lOffset = lMultisample.GetSampleOffset(s);
				vector4f lRay((((lOffset.x+j)*2.f)/(IMAGE_SIZE-1))-1, 1-(((lOffset.y+i)*2.f)/(IMAGE_SIZE-1)), 1, 1);
				vector4f lRes = TransformPoint(lTransform, lRay);
				lRes = lRes * (1.f / lRes.w);

				Vector lDir(lRes.x, lRes.y, lRes.z);
				lDir = Normalize(lDir);

				*lIter = *lIter + RGBPixel(lRaytracer.TraceCameraRay( lCameraPos, lDir, 1000 ));
			}
			*lIter = *lIter / lMultisample.GetSamplesPerPixel();
			++lIter;

			int lNewProgress = (i * IMAGE_SIZE + j) * 100 / (IMAGE_SIZE * IMAGE_SIZE);
			if(lProgess != lNewProgress)
			{
				printf("%3d%%\b\b\b\b", lNewProgress);
				lProgess = lNewProgress;
			}
		}
	}
	lEnd = GetTimeMilliseconds();
	printf("Image rendered in %d milliseconds. Traced %d rays\n", lEnd - lStart, lRaytracer.TracedRays());

	ExportToTGA(lImage, "out.tga");

	return 0;
}