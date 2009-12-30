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

using namespace pragma;

#define IMAGE_SIZE 1024

Point lLigth(0,54,0);

namespace pragma
{
	class Multisample
	{
	public:
		Multisample(size_t aSamplesPerPixel)
			: mSamplesPerPixel(aSamplesPerPixel)
		{
			mValues.reserve(mSamplesPerPixel);
			size_t lWidth = (size_t)pragma::sqrt<float>((float)mSamplesPerPixel);
			size_t lHeigth = mSamplesPerPixel / lWidth;
			vector2f lOffset( 1.f / (lWidth+1), 1.f / (lHeigth+1));
			vector2f lSample(0,0);
			for(size_t i = 0; i < lHeigth; ++i)
			{
				lSample.y+= lOffset.y;
				for(size_t j = 0; j < lWidth; ++j)
				{
					lSample.x+= lOffset.x;
					mValues.push_back(lSample);										
				}
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

		Color TraceCameraRay(const Point& aPosition, const Vector& aDirection, Real aLength)
		{
			Color lRetVal(0,0,0);
			MeshCollision::TResult lResult;

			if( mCollisionMap.IntersectRay( aPosition, aDirection, aLength, lResult ) )
			{
				// Collision, trace Lighting ray
				size_t lCount;
				const Mesh::TTriangle& lTri = mMesh.GetTriangles(lCount)[lResult.mTriangleIndex];

				Vector lToLigth = Normalize(lLigth - lResult.mPosition);
				Vector lCollisionPoint = lResult.mPosition + lToLigth * (4.f * math::type_traits<Real>::epsilon);
				bool lShadowRayCollision = mCollisionMap.IntersectRay( lCollisionPoint, lLigth );
				if( !lShadowRayCollision ) 
				{
					Vector lNormal;
					ComputeFromBarycentric( mMesh.GetVertexNormal(lTri.mVertexNormal[0])
										  , mMesh.GetVertexNormal(lTri.mVertexNormal[1])
										  , mMesh.GetVertexNormal(lTri.mVertexNormal[2])
										  , lResult.mBarycentric
										  , lNormal );
					Real lDiffuse = DotProduct(lNormal, Normalize(lLigth-lCollisionPoint));
					if(lDiffuse > 0)
					{
						lRetVal = mMaterialLibrary.GetMaterial(lTri.mMaterial).GetdiffuseColor() * lDiffuse;
					}
				}
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
	ParseOBJ("../../../scenes/Cornell.obj", lMesh, lMaterialLibrary);
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

	Color* lImage = new Color[IMAGE_SIZE*IMAGE_SIZE];
	Color* lPtr = lImage;

	lStart = GetTimeMilliseconds();

	Multisample lMultisample(1);

	Raytracer lRaytracer(lMesh, lMaterialLibrary);

	// Scanline renderer
	for(size_t i = 0; i < IMAGE_SIZE; ++i)
	{
		for(size_t j = 0; j < IMAGE_SIZE; ++j)
		{
			*lPtr = Color(0,0,0);
			// Multiple samples per pixel
			for(size_t s = 0; s < lMultisample.GetSamplesPerPixel(); ++s)
			{
				vector2f lOffset = lMultisample.GetSampleOffset(s);
				vector4f lRay((((lOffset.x+j)*2.f)/(IMAGE_SIZE-1))-1, 1-(((lOffset.y+i)*2.f)/(IMAGE_SIZE-1)), 1, 1);
				vector4f lRes = TransformPoint(lTransform, lRay);
				lRes = lRes * (1.f / lRes.w);

				Vector lDir(lRes.x, lRes.y, lRes.z);
				lDir = Normalize(lDir);

				*lPtr = *lPtr + lRaytracer.TraceCameraRay( lCameraPos, lDir, 1000 );
			}
			*lPtr = *lPtr / lMultisample.GetSamplesPerPixel();
			lPtr++;
		}
	}
	lEnd = GetTimeMilliseconds();
	//printf("Image rendered in %d milliseconds. Traced %d rays\n", lEnd - lStart, lCollisionMap.TracedRays());

	uint8* lRAWImage = new uint8[IMAGE_SIZE*IMAGE_SIZE*3];
	for(size_t i = 0; i < IMAGE_SIZE*IMAGE_SIZE; ++i)
	{
		lRAWImage[i*3+0] = uint8(lImage[i].x * 255.f);
		lRAWImage[i*3+1] = uint8(lImage[i].y * 255.f);
		lRAWImage[i*3+2] = uint8(lImage[i].z * 255.f);
	}
	FILE* handle = fopen("out.raw","wb");
	fwrite(lRAWImage, IMAGE_SIZE*IMAGE_SIZE, 3, handle);
	fclose(handle);

	delete[] lImage;
	delete[] lRAWImage;

	return 0;
}