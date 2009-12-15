#pragma once

#include <pragma/graphics/MeshCollision.h>
#include <pragma/geometry/intersection/ray_triangle.h>
#include <pragma/geometry/functions.h>

namespace pragma
{
	MeshCollision::MeshCollision(const Mesh& aMesh)
		: mMesh(aMesh)
	{

	}

	bool MeshCollision::IntersectRay( const vector3f& aOrigin, const vector3f& aDestination)
	{
		vector3f lDirection = Normalize<float>(aDestination - aOrigin);
		float lRayLength = Length<float>(aDestination - aOrigin);
		size_t lVertexCount;
		const Point* lVertexs = mMesh.GetVertexs(lVertexCount);

		size_t lTriCount;
		const Mesh::TTriangle* lTris = mMesh.GetTriangles(lTriCount);

		for(size_t i = 0; i < lTriCount; ++i, ++lTris)
		{
			float lDistance;
			vector2f lBarycentric;
			if( IntersectRayTriangle_2Sided( lVertexs[lTris->mVertex[0]], lVertexs[lTris->mVertex[1]]
										   , lVertexs[lTris->mVertex[2]], aOrigin, lDirection, lRayLength
										   , lBarycentric, lDistance ) == true )
			{
				return true;
			}

		}

		return false;
	}

	bool MeshCollision::IntersectRay( const vector3f& aOrigin, const vector3f& aDirection, float aRayLength, int& aIndex, vector2f& aBarycentric, float& aDistance)
	{
		size_t lVertexCount;
		const Point* lVertexs = mMesh.GetVertexs(lVertexCount);

		size_t lTriCount;
		const Mesh::TTriangle* lTris = mMesh.GetTriangles(lTriCount);

		aDistance = 0;
		aIndex = -1;
		for(size_t i = 0; i < lTriCount; ++i, ++lTris)
		{
			float lDistance;
			vector2f lBarycentric;
			if( IntersectRayTriangle_2Sided( lVertexs[lTris->mVertex[0]], lVertexs[lTris->mVertex[1]]
									, lVertexs[lTris->mVertex[2]], aOrigin, aDirection, aRayLength
									, lBarycentric, lDistance ) == true )
			{
				if( lDistance < aDistance ||
					aIndex == -1 )
				{
					aDistance = lDistance;
					aBarycentric = lBarycentric;
					aIndex = (int)i;
				}
			}

		}

		return aIndex != -1;
	}
}
