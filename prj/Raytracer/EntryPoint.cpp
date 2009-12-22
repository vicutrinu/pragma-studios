#include <pragma/graphics/Mesh.h>
#include <pragma/graphics/import/ASE_Parser.h>
#include <pragma/graphics/Camera.h>
#include <pragma/graphics/MeshCollision.h>

#include <pragma/math/types.h>
#include <pragma/math/functions.h>

#include <pragma/geometry/functions.h>

#include <stdio.h>

using namespace pragma;

#define IMAGE_SIZE 512

int main(int argc, char* argv[])
{
	// Load ASE file
	pragma::Mesh lMesh;
	ParseASE("scenes\\Torus.ASE", lMesh);

	Vector lCameraPos(30,30,30);
	Camera lCamera;
	lCamera.SetProjection(45, 1, 1.f, 300.f);
	lCamera.SetTransform( lCameraPos, Vector(0,0,0), Vector(0,1,0) );

	matrix4x4f lTransform = lCamera.GetProjection() * lCamera.GetTransform();
	lTransform = Inverse(lTransform);

	FILE* handle = fopen("out.raw","wb");

	MeshCollision lCollisionMap(lMesh);

	Point lLigth(-50,50,0);

	size_t lVertexCount;
	const Point* lVertexs = lMesh.GetVertexs(lVertexCount);

	for(size_t i = 0; i < IMAGE_SIZE; ++i)
	{
		for(size_t j = 0; j < IMAGE_SIZE; ++j)
		{
			vector4f lRay(((j*2.f)/(IMAGE_SIZE-1))-1, 1-((i*2.f)/(IMAGE_SIZE-1)), 1, 1);
			vector4f lRes = TransformPoint(lTransform, lRay);
			lRes = lRes * (1.f / lRes.w);

			Vector lDir(lRes.x, lRes.y, lRes.z);
			lDir = Normalize(lDir);

			Vector2 lOut;
			Real lDistance;
			int lTriIndex;

			if( lCollisionMap.IntersectRay( lCameraPos, lDir, 1000, lTriIndex, lOut, lDistance ) )
			{
				size_t lCount;
				const Mesh::TTriangle& lTri = lMesh.GetTriangles(lCount)[lTriIndex];
				Point lCollisionPoint = lVertexs[lTri.mVertex[0]] * Real(1.f-lOut.x-lOut.y) + 
										lVertexs[lTri.mVertex[1]] * Real(lOut.x) + 
										lVertexs[lTri.mVertex[2]] * Real(lOut.y);
				lCollisionPoint = lCollisionPoint + Normalize<Real>(lCollisionPoint-lLigth) * (4.f * math::type_traits<Real>::epsilon);
				if(lCollisionMap.IntersectRay( lCollisionPoint, lLigth ))
				{
					putc(lDistance * 255. / 200, handle); putc(0, handle); putc(0, handle);
				}
				else
				{
					Vector lNormal = lMesh.GetVertexNormal(lTri.mVertexNormal[0]) * Real(1.f-lOut.x-lOut.y) + 
									 lMesh.GetVertexNormal(lTri.mVertexNormal[1]) * Real(lOut.x) + 
									 lMesh.GetVertexNormal(lTri.mVertexNormal[2]) * Real(lOut.y);
					//lNormal = lMesh.GetTriangleNormal(lTriIndex);
					float lPollas = DotProduct(lNormal, Normalize(lLigth-lCollisionPoint));
					if(lPollas > 0)
					{
						putc(lDistance * 255. / 200, handle); putc(lPollas*255, handle); putc(255, handle);
					}
					else
					{
						putc(lDistance * 255. / 200, handle); putc(0, handle); putc(255, handle);
					}
				}
			}
			else
			{
				putc(0, handle); putc(0, handle); putc(0, handle);
			}
		}
	}

	fclose(handle);

	return 0;
}