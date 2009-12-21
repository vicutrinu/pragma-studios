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

	bool MeshCollision::IntersectRay( const Point& aOrigin, const Point& aDestination)
	{
		Vector lDirection = Normalize<Real>(aDestination - aOrigin);
		Real lRayLength = Length<Real>(aDestination - aOrigin);
		size_t lVertexCount;
		const Point* lVertexs = mMesh.GetVertexs(lVertexCount);

		size_t lTriCount;
		const Mesh::TTriangle* lTris = mMesh.GetTriangles(lTriCount);

		for(size_t i = 0; i < lTriCount; ++i, ++lTris)
		{
			Real lDistance;
			Vector2 lBarycentric;
			if( IntersectRayTriangle( lVertexs[lTris->mVertex[0]], lVertexs[lTris->mVertex[1]]
									, lVertexs[lTris->mVertex[2]], aOrigin, lDirection, lRayLength
									, lBarycentric, lDistance ) == true && 
				lDistance >= 0 )
			{
				return true;
			}

		}

		return false;
	}

	bool MeshCollision::IntersectRay( const Point& aOrigin, const Vector& aDirection, Real aRayLength, int& aIndex, Vector2& aBarycentric, Real& aDistance)
	{
		size_t lVertexCount;
		const Point* lVertexs = mMesh.GetVertexs(lVertexCount);

		size_t lTriCount;
		const Mesh::TTriangle* lTris = mMesh.GetTriangles(lTriCount);

		aDistance = 0;
		aIndex = -1;
		for(size_t i = 0; i < lTriCount; ++i, ++lTris)
		{
			Real lDistance;
			Vector2 lBarycentric;
			if( IntersectRayTriangle( lVertexs[lTris->mVertex[0]], lVertexs[lTris->mVertex[1]]
									, lVertexs[lTris->mVertex[2]], aOrigin, aDirection, aRayLength
									, lBarycentric, lDistance ) == true &&
				lDistance >= 0)
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
