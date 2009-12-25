#pragma once

#include <pragma/graphics/MeshCollision.h>
#include <pragma/geometry/intersection/ray_triangle.h>
#include <pragma/geometry/intersection/ray_box.h>
#include <pragma/geometry/functions.h>
#include <pragma/math/functions.h>

namespace pragma
{
	MeshCollision::MeshCollision(const Mesh& aMesh)
		: mMesh(aMesh)
	{
		BuildKdTree();
	}

	bool MeshCollision::IntersectRay( const Point& aOrigin, const Point& aDestination)
	{
		if(!IntersectRayBox(mMesh.GetBoundingBox(), aOrigin, Normalize(aDestination-aOrigin)))
			return false;

		Vector lDirection = Normalize<Real>(aDestination - aOrigin);


		std::vector<int> lTriangleList;
		WalkKdTree(aOrigin, aDestination, lDirection, *mRootNode, lTriangleList);

		size_t lVertexCount;
		const Point* lVertexs = mMesh.GetVertexs(lVertexCount);
		Real lRayLength = Length<Real>(aDestination - aOrigin);

		for(size_t i = 0; i < lTriangleList.size(); ++i)
		{
			const Mesh::TTriangle& lTri = mMesh.GetTriangle(lTriangleList[i]);
			Real lDistance;
			Vector2 lBarycentric;
			if( IntersectRayTriangle( lVertexs[lTri.mVertex[0]], lVertexs[lTri.mVertex[1]]
									, lVertexs[lTri.mVertex[2]], aOrigin, lDirection, lRayLength
									, lBarycentric, lDistance ) == true && 
				lDistance >= 0 )
			{
				return true;
			}

		}

		return false;
	}

	void MeshCollision::WalkKdTree(const Point& aOrigin, const Point& aDestination, const Vector& aDirection, const TNode& aNode, std::vector<int>& aTriangleList)
	{
		if(aNode.mLeft == -1)
		{
			Real aDistance;
			Vector2 aBarycentric;
			int aIndex;
			if(IntersectRayTriangleList(aOrigin, aDirection, Length(aDestination-aOrigin), aNode.mTriangles, aIndex, aBarycentric, aDistance))
			{
				aTriangleList.push_back(aIndex);
			}

			/*for(size_t i = 0; i < aNode.mTriangles.size(); ++i)
			{
				bool lFound = false;
				for(size_t j = 0; j < aTriangleList.size(); ++j)
				{
					if(aNode.mTriangles[i] == aTriangleList[j])
					{
						lFound = true;
						break;
					}
				}
				if(!lFound)
					aTriangleList.push_back(aNode.mTriangles[i]);
			}*/
			return;
		}

		int lPlane = aNode.mSplitAxis;
		if(aOrigin.i[lPlane] < aNode.mSplitPlane)
		{
			if(aDestination.i[lPlane] < aNode.mSplitPlane)
			{
				WalkKdTree(aOrigin, aDestination, aDirection, mNodes[aNode.mLeft], aTriangleList);
			}
			else
			{
				Real lScale = (aNode.mSplitPlane - aOrigin.i[lPlane]) / aDirection.i[lPlane];
				Point lIntersection = aOrigin + (aDirection * lScale);
				WalkKdTree(aOrigin, lIntersection, aDirection, mNodes[aNode.mLeft], aTriangleList);
				WalkKdTree(lIntersection, aDestination, aDirection, mNodes[aNode.mRight], aTriangleList);
			}
		}
		else
		{
			if(aDestination.i[lPlane] < aNode.mSplitPlane)
			{
				Real lScale = (aNode.mSplitPlane - aOrigin.i[lPlane]) / aDirection.i[lPlane];
				Point lIntersection = aOrigin + (aDirection * lScale);
				WalkKdTree(aOrigin, lIntersection, aDirection, mNodes[aNode.mRight], aTriangleList);
				WalkKdTree(lIntersection, aDestination, aDirection, mNodes[aNode.mLeft], aTriangleList);
			}
			else
			{
				WalkKdTree(aOrigin, aDestination, aDirection, mNodes[aNode.mRight], aTriangleList);
			}
		}
	}

	bool MeshCollision::IntersectRay( const Point& aOrigin, const Vector& aDirection, Real aRayLength, int& aIndex, Vector2& aBarycentric, Real& aDistance)
	{
		if(!IntersectRayBox(mMesh.GetBoundingBox(), aOrigin, aDirection))
			return false;

		Point lDestination = aOrigin + (aDirection * aRayLength);
		std::vector<int> lTriangleList;
		WalkKdTree(aOrigin, lDestination, aDirection, *mRootNode, lTriangleList);

		return IntersectRayTriangleList(aOrigin, aDirection, aRayLength, lTriangleList, aIndex, aBarycentric, aDistance);
	}

	bool MeshCollision::IntersectRayTriangleList(const Point& aOrigin, const Point& aDirection, Real aRayLength, std::vector<int> aTriangleList, int& aIndex, Vector2& aBarycentric, Real& aDistance)
	{
		size_t lVertexCount;
		const Point* lVertexs = mMesh.GetVertexs(lVertexCount);

		aDistance = 0;
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
				if( lDistance < aDistance ||
					aIndex == -1 )
				{
					aDistance = lDistance;
					aBarycentric = lBarycentric;
					aIndex = aTriangleList[i];
				}
			}

		}

		return aIndex != -1;
	}

	void MeshCollision::BuildKdTree()
	{
		mNodes.reserve(4096);
		mNodes.push_back(TNode());
		mRootNode = &mNodes.back();

		mRootNode->mTriangles.reserve(mMesh.GetNumTriangles());
		std::vector<int>& lTriangles = mRootNode->mTriangles;
		for(size_t i = 0; i < mMesh.GetNumTriangles(); ++i)
		{
			lTriangles.push_back(i);
			mTriangles.push_back(TTriangle());
			TTriangle& lTri = mTriangles.back();
			mMesh.GetTriangleVertexs(i, lTri.mA, lTri.mB, lTri.mC);
			lTri.mCentroid = (lTri.mA + lTri.mB + lTri.mC) / Real(3);
		}

		Split(*mRootNode, 0);
	}

	void MeshCollision::Split(TNode& aNode, int aPlane)
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

		if( mNodes[aNode.mLeft].mTriangles.size() > 1 &&
			mNodes[aNode.mLeft].mTriangles.size() > 200 )
			Split(mNodes[aNode.mLeft], (aPlane+1)%3);

		if( mNodes[aNode.mRight].mTriangles.size() > 1 && 
			mNodes[aNode.mRight].mTriangles.size() > 200 )
			Split(mNodes[aNode.mRight], (aPlane+1)%3);
	}

}
