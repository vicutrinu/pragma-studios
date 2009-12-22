#pragma once

#include <pragma/graphics/Mesh.h>

namespace pragma
{
	class MeshCollision
	{
	public:
		MeshCollision(const Mesh& aMesh);

		bool IntersectRay( const Point& aOrigin, const Vector& aDirection, Real aRayLength, int& aIndex, Vector2& aBarycentric, Real& aDistance );
		bool IntersectRay( const Point& aOrigin, const Point& aDestination );

	private:
		void BuildKdTree();

	private:

		struct TTriangle
		{
			Vector	mCentroid;
			Vector	mA, mB, mC;
		};

		struct TNode
		{
			std::vector<int>	mTriangles;
			int					mSplitAxis;
			Real				mSplitPlane;
			int					mLeft;
			int					mRight;
		};

		std::vector<TTriangle>	mTriangles;
		std::vector<TNode>		mNodes;
		TNode*					mRootNode;

		const Mesh& mMesh;

		void Split(TNode& aNode, int aPlane);
	};
}
