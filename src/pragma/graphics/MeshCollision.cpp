#pragma once

#include <pragma/graphics/MeshCollision.h>
#include <pragma/geometry/intersection/ray_triangle.h>
#include <pragma/geometry/intersection/ray_box.h>
#include <pragma/geometry/functions.h>
#include <pragma/math/functions.h>

#include <stdio.h>

namespace pragma
{
	MeshCollision::MeshCollision(const Mesh& aMesh)
		: mMesh(aMesh)
	{
		BuildKdTree(10, 50);
	}

	bool MeshCollision::IntersectRay( const Point& aOrigin, const Vector& aDirection, Real aRayLength, TResult& aResult )
	{
		if(!IntersectRayBox(mMesh.GetBoundingBox(), aOrigin, aDirection))
			return false;

		Point lDestination = aOrigin + (aDirection * aRayLength);
		std::vector<int> lTriangleList;

		if( WalkKdTree(aOrigin, lDestination, aDirection, mNodes[0]) )
		{
			aResult.mTriangleIndex	= mLastResult.mTriangleIndex;
			aResult.mBarycentric	= mLastResult.mBarycentric;

			size_t lCount;
			const Mesh::TTriangle& lTri = mMesh.GetTriangles(lCount)[aResult.mTriangleIndex];
			size_t lVertexCount;
			const Point* lVertexs = mMesh.GetVertexs(lVertexCount);

			ComputeFromBarycentric( lVertexs[ lTri.mVertex[0] ], lVertexs[ lTri.mVertex[1] ], lVertexs[ lTri.mVertex[2] ]
								  , aResult.mBarycentric, aResult.mPosition );

			return true;
		}
		else
			return false;
	}

	bool MeshCollision::IntersectRay( const Point& aOrigin, const Point& aDestination)
	{
		if(!IntersectRayBox(mMesh.GetBoundingBox(), aOrigin, Normalize(aDestination-aOrigin)))
			return false;

		Vector lDirection = Normalize<Real>(aDestination - aOrigin);

		return WalkKdTree(aOrigin, aDestination, lDirection, mNodes[0]);
	}

	bool MeshCollision::WalkKdTree( const Point& aOrigin, const Point& aDestination, const Vector& aDirection, const TNode& aNode )
	{
		if(aNode.mLeft == -1)
		{
			Vector2 lBarycentric;
			int lIndex;
			if(IntersectRayTriangleList(aOrigin, aDirection, Length(aDestination-aOrigin), aNode.mTriangles, lIndex, lBarycentric))
			{
				mLastResult.mTriangleIndex = lIndex;
				mLastResult.mBarycentric = lBarycentric;
				return true;
			}
			return false;			
		}

		int lPlane = aNode.mSplitAxis;
		if(aOrigin.i[lPlane] < aNode.mSplitPlane)
		{
			if(aDestination.i[lPlane] < aNode.mSplitPlane)
			{
				return WalkKdTree(aOrigin, aDestination, aDirection, mNodes[aNode.mLeft]);
			}
			else
			{
				Real lScale = (aNode.mSplitPlane - aOrigin.i[lPlane]) / aDirection.i[lPlane];
				Point lIntersection = aOrigin + (aDirection * lScale);
				if( !WalkKdTree(aOrigin, lIntersection, aDirection, mNodes[aNode.mLeft]) &&
					!WalkKdTree(lIntersection, aDestination, aDirection, mNodes[aNode.mRight]) )
					return false;
				else
					return true;
			}
		}
		else
		{
			if(aDestination.i[lPlane] < aNode.mSplitPlane)
			{
				Real lScale = (aNode.mSplitPlane - aOrigin.i[lPlane]) / aDirection.i[lPlane];
				Point lIntersection = aOrigin + (aDirection * lScale);

				Vector lDir2 = Normalize(lIntersection - aOrigin);

				if( !WalkKdTree(aOrigin, lIntersection, aDirection, mNodes[aNode.mRight]) &&
					!WalkKdTree(lIntersection, aDestination, aDirection, mNodes[aNode.mLeft]) )
					return false;
				else
					return true;
			}
			else
				return WalkKdTree(aOrigin, aDestination, aDirection, mNodes[aNode.mRight]);
		}
	}

	bool MeshCollision::IntersectRayTriangleList(const Point& aOrigin, const Point& aDirection, Real aRayLength, std::vector<int> aTriangleList, int& aIndex, Vector2& aBarycentric)
	{
		size_t lVertexCount;
		const Point* lVertexs = mMesh.GetVertexs(lVertexCount);

		Real aNearesDistance = 0;
		aIndex = -1;

		for(size_t i = 0; i < aTriangleList.size(); ++i)
		{
			const Mesh::TTriangle& lTri = mMesh.GetTriangle(aTriangleList[i]);
			Real lDistance;
			Vector2 lBarycentric;
			if( IntersectRayTriangle( lVertexs[lTri.mVertex[0]], lVertexs[lTri.mVertex[1]]
									, lVertexs[lTri.mVertex[2]], aOrigin, aDirection, aRayLength
									, lBarycentric, lDistance ) == true &&
				lDistance >= 0)
			{
				if( lDistance < aNearesDistance ||
					aIndex == -1 )
				{
					aNearesDistance = lDistance;
					aBarycentric = lBarycentric;
					aIndex = aTriangleList[i];
				}
			}
		}

		mTracedRays+= aTriangleList.size();

		return aIndex != -1;
	}

	void MeshCollision::BuildKdTree( size_t aDepth, size_t aMaxPolysPerNode )
	{
		aDepth = min<size_t>(aDepth, 10);
		mNodes.reserve((uint32)1<<(aDepth*2));
		mNodes.push_back(TNode());

		TNode& lRootNode = mNodes.back();
		lRootNode.mTriangles.reserve(mMesh.GetNumTriangles());
		std::vector<int>& lTriangles = lRootNode.mTriangles;
		for(size_t i = 0; i < mMesh.GetNumTriangles(); ++i)
		{
			lTriangles.push_back(i);
			mTriangles.push_back(TTriangle());
			TTriangle& lTri = mTriangles.back();
			mMesh.GetTriangleVertexs(i, lTri.mA, lTri.mB, lTri.mC);
			lTri.mCentroid = (lTri.mA + lTri.mB + lTri.mC) / Real(3);
		}

		mMaxPolysPerNode = aMaxPolysPerNode;
		Split(lRootNode, 0, aDepth);
	}

	void MeshCollision::Split(TNode& aNode, int aPlane, size_t aDepth)
	{
		aNode.mSplitAxis = aPlane;
		aNode.mSplitPlane = 0;

		Real lPlane = 0;
		for(size_t i = 0; i < aNode.mTriangles.size(); ++i)
		{
			lPlane+= mTriangles[aNode.mTriangles[i]].mCentroid.i[aPlane];
		}
		aNode.mSplitPlane = lPlane / aNode.mTriangles.size();
		
  		aNode.mLeft = mNodes.size();
		mNodes.push_back(TNode());
		mNodes.back().mLeft = -1;
		mNodes.back().mRight = -1;

		aNode.mRight = mNodes.size();
		mNodes.push_back(TNode());
		mNodes.back().mLeft = -1;
		mNodes.back().mRight = -1;

		std::vector<int>& lLeft = mNodes[aNode.mLeft].mTriangles;
		std::vector<int>& lRight = mNodes[aNode.mRight].mTriangles;
		lLeft.reserve( aNode.mTriangles.size() );
		lRight.reserve( aNode.mTriangles.size() );
		for(size_t i = 0; i < aNode.mTriangles.size(); ++i)
		{
			const TTriangle& lTri = mTriangles[aNode.mTriangles[i]];
			if(lTri.mCentroid.i[aPlane] < aNode.mSplitPlane)
				lLeft.push_back(aNode.mTriangles[i]);
			else
				lRight.push_back(aNode.mTriangles[i]);
		}

		size_t lRightSize = lRight.size();

		for(size_t i = 0; i < lLeft.size(); ++i)
		{
			TTriangle& lTri = mTriangles[lLeft[i]];
			if(lTri.mA.i[aPlane] >= aNode.mSplitPlane || lTri.mB.i[aPlane] >= aNode.mSplitPlane || lTri.mC.i[aPlane] >= aNode.mSplitPlane)
				lRight.push_back(lLeft[i]);
		}

		for(size_t i = 0; i < lRightSize; ++i)
		{
			TTriangle& lTri = mTriangles[lRight[i]];
			if(lTri.mA.i[aPlane] < aNode.mSplitPlane || lTri.mB.i[aPlane] < aNode.mSplitPlane || lTri.mC.i[aPlane] < aNode.mSplitPlane)
				lLeft.push_back(lRight[i]);
		}

		aNode.mTriangles.clear();

		if( mNodes[aNode.mLeft].mTriangles.size() > 1 &&
			mNodes[aNode.mLeft].mTriangles.size() > mMaxPolysPerNode &&
			aDepth > 0 )
			Split(mNodes[aNode.mLeft], (aPlane+1)%3, aDepth-1);

		if( mNodes[aNode.mRight].mTriangles.size() > 1 && 
			mNodes[aNode.mRight].mTriangles.size() > mMaxPolysPerNode &&
			aDepth > 0 )
			Split(mNodes[aNode.mRight], (aPlane+1)%3, aDepth-1);

	}

}
