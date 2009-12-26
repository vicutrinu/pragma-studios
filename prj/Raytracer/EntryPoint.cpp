#include <pragma/graphics/Mesh.h>
#include <pragma/graphics/import/ASE_Parser.h>
#include <pragma/graphics/Camera.h>
#include <pragma/graphics/MeshCollision.h>

#include <pragma/math/types.h>
#include <pragma/math/functions.h>

#include <pragma/geometry/functions.h>

#include <stdio.h>

#include <pragma/system/clock.h>

using namespace pragma;

#define IMAGE_SIZE 512

int main(int argc, char* argv[])
{
	// Load ASE file
	TMillisecond lStart = GetTimeMilliseconds();
	pragma::Mesh lMesh;
	ParseASE("scenes\\ComplexTorus.ASE", lMesh);
	TMillisecond lEnd = GetTimeMilliseconds();
	printf("Parsed ASE file in %d milliseconds\n", lEnd - lStart);

	Vector lCameraPos(30,30,30);
	Camera lCamera;
	lCamera.SetProjection(45, 1, 1.f, 300.f);
	lCamera.SetTransform( lCameraPos, Vector(1,0,0), Vector(0,1,0) );

	matrix4x4f lTransform = lCamera.GetProjection() * lCamera.GetTransform();
	lTransform = Inverse(lTransform);

	lStart = GetTimeMilliseconds();
	MeshCollision lCollisionMap(lMesh);
	lEnd = GetTimeMilliseconds();
	printf("Created KdTree in %d milliseconds\n", lEnd - lStart);

	Point lLigth(0,100,0);

	size_t lVertexCount;
	const Point* lVertexs = lMesh.GetVertexs(lVertexCount);

	char* lImage = new char[IMAGE_SIZE*IMAGE_SIZE*3];
	char* lPtr = lImage;

	lStart = GetTimeMilliseconds();
	lCollisionMap.ResetStats();
	for(size_t i = 0; i < IMAGE_SIZE; ++i)
	{
		for(size_t j = 0; j < IMAGE_SIZE; ++j)
		{
			vector4f lRay(((j*2.f)/(IMAGE_SIZE-1))-1, 1-((i*2.f)/(IMAGE_SIZE-1)), 1, 1);
			vector4f lRes = TransformPoint(lTransform, lRay);
			lRes = lRes * (1.f / lRes.w);

			Vector lDir(lRes.x, lRes.y, lRes.z);
			lDir = Normalize(lDir);

			MeshCollision::TResult lResult;

			if( lCollisionMap.IntersectRay( lCameraPos, lDir, 1000, lResult ) )
			{
				size_t lCount;
				const Mesh::TTriangle& lTri = lMesh.GetTriangles(lCount)[lResult.mTriangleIndex];
				
				Vector lCollisionPoint = lResult.mPosition + Normalize<Real>(lResult.mPosition-lLigth) * (4.f * math::type_traits<Real>::epsilon);
				if(lCollisionMap.IntersectRay( lCollisionPoint, lLigth ))
				{
					lPtr[0] = 0; lPtr[1] = 0; lPtr[2] = lResult.mTriangleIndex*25;
				}
				else
				{
					Vector lNormal;
					ComputeFromBarycentric( lMesh.GetVertexNormal(lTri.mVertexNormal[0])
										  , lMesh.GetVertexNormal(lTri.mVertexNormal[1])
										  , lMesh.GetVertexNormal(lTri.mVertexNormal[2])
										  , lResult.mBarycentric
										  , lNormal );
					//lNormal = lMesh.GetTriangleNormal( lResult.mTriangleIndex );
					/*
					Point lA, lB, lC;
					lMesh.GetTriangleVertexs(lResult.mTriangleIndex, lA, lB, lC);
					lNormal = Normalize( CrossProduct( lB - lA, lC - lA ) );
					float lDiffuse = DotProduct(lNormal, Normalize(lLigth-lCollisionPoint));
					*/
					if(lDiffuse > 0)
					{
						lPtr[0] = 255; lPtr[1] = lDiffuse*255; lPtr[2] = lResult.mTriangleIndex*25;
					}
					else
					{
						lPtr[0] = 255; lPtr[1] = 0; lPtr[2] = lResult.mTriangleIndex*25;
					}
				}
			}
			else
			{
				lPtr[0] = 0; lPtr[1] = 0; lPtr[2] = 0;
			}
			lPtr+= 3;
		}
	}
	lEnd = GetTimeMilliseconds();
	printf("Image rendered in %d milliseconds. Traced %d rays\n", lEnd - lStart, lCollisionMap.TracedRays());

	FILE* handle = fopen("out.raw","wb");
	fwrite(lImage, IMAGE_SIZE*IMAGE_SIZE, 3, handle);
	fclose(handle);

	delete[] lImage;

	return 0;
}