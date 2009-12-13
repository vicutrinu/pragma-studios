#include <stdio.h>
#include <vector>
#include <algorithm>

#include "pragma/math/types.h"
#include "pragma/math/math.h"
#include "pragma/geometry/types.h"
#include "pragma/geometry/functions.h"
#include "pragma/math/UnitTest.h"

#include "pragma/geometry/intersection/ray_triangle.h"
#include "pragma/geometry/UnitTest.h"

#include "pragma/graphics/UnitTest.h"
#include "pragma/graphics/import/ASE_Parser.h"
#include "pragma/graphics/Mesh.h"

#include "pragma/graphics/Camera.h"

using namespace pragma;

bool OrderX(const AABB* aSrc1, const AABB* aSrc2)
{
	if(aSrc1->mMax.x < aSrc2->mMin.x)
		return true;
	else 
		return false;
}

struct TResult
	{
		float mSplit;
		int mLeft;
		int mRight;
		int mBoth;
		float mScore;
	};

	bool operator<(const TResult& aSrc1, const TResult& aSrc2)
	{
		if(aSrc1.mScore < aSrc2.mScore)
			return true;
		else
			return false;
	}

class CKdTree
{
	enum ESplitPlane
	{
		eX = 0,
		eY,
		eZ
	};

	struct TNode
	{
		ESplitPlane		mSplitPlane;
		float			mSplitPosition;
		std::vector<const AABB*> mBoxes;
		size_t			mRight;
		size_t			mLeft;
	};

public:
	CKdTree()
	{
		mNodes.push_back(TNode());
		mNodes.reserve(1024);
		mBoxes.reserve(1024);
		mRoot = &mNodes.back();
	}

	void AddBox(void* aUserData, const AABB& aBox)
	{
		mBoxes.push_back(aBox);
		mRoot->mBoxes.push_back(&mBoxes.back());
	}

	void Build()
	{
		Split(mNodes[0], eX);		
	}

	int CheckX(const AABB* aBox, float aPos)
	{
		if(aBox->mMax.x < aPos)
			return -1;
		else if(aBox->mMin.x >= aPos)
			return 1;
		else
			return 0;
	}

	int CheckY(const AABB* aBox, float aPos)
	{
		if(aBox->mMax.y < aPos)
			return -1;
		else if(aBox->mMin.y >= aPos)
			return 1;
		else
			return 0;
	}

	void CountOptionX(const std::vector<const AABB*> aList, float aSplitX, TResult& aResult)
	{
		aResult.mSplit = aSplitX;
		aResult.mBoth = aResult.mRight = aResult.mLeft = 0;
		for(size_t i = 0; i < aList.size(); ++i)
		{
			if(aList[i]->mMax.x <= aSplitX)
				aResult.mLeft++;
			else if(aList[i]->mMin.x >= aSplitX)
				aResult.mRight++;
			else
				aResult.mBoth++;
		}
		aResult.mScore = abs(float(aResult.mRight - aResult.mLeft)) / 2;
		aResult.mScore+= aResult.mBoth;
	}

	void CountOptionY(const std::vector<const AABB*> aList, float aSplitY, TResult& aResult)
	{
		aResult.mSplit = aSplitY;
		aResult.mBoth = aResult.mRight = aResult.mLeft = 0;
		for(size_t i = 0; i < aList.size(); ++i)
		{
			if(aList[i]->mMax.y <= aSplitY)
				aResult.mLeft++;
			else if(aList[i]->mMin.y >= aSplitY)
				aResult.mRight++;
			else
				aResult.mBoth++;
		}
		aResult.mScore = abs(float(aResult.mRight - aResult.mLeft)) / 2;
		aResult.mScore+= aResult.mBoth;
	}

	void Split(TNode& aNode, ESplitPlane aPlane)
	{
		ESplitPlane lNextPlane;
		if(aPlane == eX)
			lNextPlane = eY;
		else if(aPlane == eY)
			lNextPlane = eX;

		if(aNode.mBoxes.size() > 1)
		{
			aNode.mLeft = mNodes.size();
			mNodes.push_back(TNode());
			TNode& lLeft = mNodes.back();

			aNode.mRight = mNodes.size();
			mNodes.push_back(TNode());
			TNode& lRight = mNodes.back();

			aNode.mSplitPlane = aPlane;

			// TODO: Calcular el mejor punto de corte
			// El mejor punto de corte es aquel que mejor divide en dos todas las cajas
			// Una caja puede estar al lado derecho, al lado izquierdo o en los dos las dos a la vez
			// El mejor caso es que haya el mismo numero de cajas a la derecha que a la izquierda, y cero cajas en los dos lados al mismo tiempo
			std::vector<TResult> lResults;
				lResults.reserve(aNode.mBoxes.size() * 2);
			if(aPlane == eX)
			{
				for(size_t i = 0; i < aNode.mBoxes.size(); ++i)
				{
					TResult lResult;
					CountOptionX(aNode.mBoxes, aNode.mBoxes[i]->mMin.x, lResult);
					lResults.push_back(lResult);
					CountOptionX(aNode.mBoxes, aNode.mBoxes[i]->mMax.x, lResult);
					lResults.push_back(lResult);
				}

				std::sort(lResults.begin(), lResults.end());
				aNode.mSplitPosition = lResults[0].mSplit;

				for(size_t i = 0; i < aNode.mBoxes.size(); ++i)
				{
					switch( CheckX(aNode.mBoxes[i], aNode.mSplitPosition) )
					{
					case -1: lLeft.mBoxes.push_back(aNode.mBoxes[i]); break;
					case 1:  lRight.mBoxes.push_back(aNode.mBoxes[i]); break;
					case 0:
					default: 
						lLeft.mBoxes.push_back(aNode.mBoxes[i]);
						lRight.mBoxes.push_back(aNode.mBoxes[i]);
						break;

					}
				}
			}
			else
			{
				for(size_t i = 0; i < aNode.mBoxes.size(); ++i)
				{
					TResult lResult;
					CountOptionY(aNode.mBoxes, aNode.mBoxes[i]->mMin.y, lResult);
					lResults.push_back(lResult);
					CountOptionY(aNode.mBoxes, aNode.mBoxes[i]->mMax.y, lResult);
					lResults.push_back(lResult);
				}

				std::sort(lResults.begin(), lResults.end());
				aNode.mSplitPosition = lResults[0].mSplit;

				for(size_t i = 0; i < aNode.mBoxes.size(); ++i)
				{
					switch( CheckY(aNode.mBoxes[i], aNode.mSplitPosition) )
					{
					case -1: lLeft.mBoxes.push_back(aNode.mBoxes[i]); break;
					case 1:  lRight.mBoxes.push_back(aNode.mBoxes[i]); break;
					case 0:
					default: 
						lLeft.mBoxes.push_back(aNode.mBoxes[i]);
						lRight.mBoxes.push_back(aNode.mBoxes[i]);
						break;

					}
				}
			}

			Split(lLeft, lNextPlane);
			Split(lRight, lNextPlane);
		}
		else
			aNode.mLeft = aNode.mRight = 0;

	}

	bool IsPointInsideBox(const vector3f& aPoint)
	{
		return IsPointInsideBox(mRoot, aPoint);
	}

	bool IsPointInsideBox(const TNode* aNode, const vector3f& aPoint)
	{
		if(aNode->mLeft == 0|| aNode->mRight == 0)
		{
			for(size_t i = 0; i < aNode->mBoxes.size(); ++i)
			{
				if( PointInsideAABB(*aNode->mBoxes[i], aPoint) )
					return true;
			}
			return false;
		}
		else
		{
			if(aNode->mSplitPlane == eX)
			{
				if(aPoint.x < aNode->mSplitPosition)
					return IsPointInsideBox(&mNodes[aNode->mLeft], aPoint);
				else
					return IsPointInsideBox(&mNodes[aNode->mRight], aPoint);
			}
			else if(aNode->mSplitPlane == eY)
			{
				if(aPoint.y < aNode->mSplitPosition)
					return IsPointInsideBox(&mNodes[aNode->mLeft], aPoint);
				else
					return IsPointInsideBox(&mNodes[aNode->mRight], aPoint);
			}
			return false;
		}
	}

private:
	std::vector<TNode>	mNodes;
	TNode*				mRoot;
	std::vector<AABB>	mBoxes;
};

int main(int argc, char* argv[])
{
#if defined PRAGMA_UNIT_TESTS && PRAGMA_UNIT_TESTS == 1
	pragma::DoMathTests();
	pragma::DoIntersectionTests();
	pragma::DoGraphicsTests();
#endif

	// Load ASE file
	pragma::Mesh lMesh;
	ParseASE("../src/pragma/tests/Torus.ASE", lMesh);

	Camera lCamera;
	lCamera.SetProjection(45, 1, 0.1f, 100.f);
	lCamera.SetTransform( Point(0,0,-5), Vector(0,0,-1), Vector(0,1,0) );

	matrix4x4f lTransform = lCamera.GetProjection() * lCamera.GetTransform();
	Inverse(lTransform);

	for(size_t i = 0; i < 512; ++i)
	{
		for(size_t j = 0; j < 512; ++j)
		{
			
		}
	}

	CKdTree lMap;

	lMap.AddBox(0, AABB( vector3f(20,0,0), vector3f(40, 10, 10) ));
	lMap.AddBox(0, AABB( vector3f(0,0,0), vector3f(10, 10, 10) ));
	
	lMap.Build();

	lMap.IsPointInsideBox(vector3f(5,5,5));

	return 0;
}