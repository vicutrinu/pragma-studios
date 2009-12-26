#pragma once

#include <pragma/graphics/Mesh.h>
#include <pragma/system/clock.h>

namespace pragma
{
	class MeshCollision
	{
	public:
		struct TResult
		{
			int		mTriangleIndex;
			Vector2 mBarycentric;
			Vector	mPosition;
		};

	public:
				MeshCollision	( const Mesh& aMesh );

		bool	IntersectRay	( const Point& aOrigin, const Vector& aDirection, Real aRayLength, TResult& aResult );
		bool	IntersectRay	( const Point& aOrigin, const Point& aDestination );

		void	ResetStats		() { mTracedRays = 0; }
		size_t	TracedRays		() const { return mTracedRays; }

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
		size_t					mMaxPolysPerNode;
		TResult					mLastResult;

		const Mesh& mMesh;

		void	BuildKdTree					( size_t aDepth, size_t aMaxPolysPerNode );
		void	Split						( TNode& aNode, int aPlane, size_t aDepth );

	private:
		bool	WalkKdTree					( const Point& aOrigin, const Point& aDestination, const Vector& aDirection, const TNode& aNode );
		bool	IntersectRayTriangleList	( const Point& aOrigin, const Point& aDirection, Real aRayLength, std::vector<int> aTriangleList, int& aIndex, Vector2& aBarycentric );

		// stats
		size_t		 mTracedRays;
	};
}
