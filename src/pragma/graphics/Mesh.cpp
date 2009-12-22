#include "types.h"
#include "Mesh.h"

#include <pragma/math/functions.h>
#include <pragma/geometry/functions.h>

namespace pragma
{
	Mesh::Mesh(const char* aName)
		: mName(aName)
	{

	}

	void Mesh::SetVertexs( const Point* aVertexs, size_t aCount )
	{
		mVertexs.clear();
		mVertexs.reserve(aCount);
		mVertexs.insert(mVertexs.begin(), aVertexs, aVertexs+aCount);
	}

	void Mesh::SetTriangles( const TTriangle* aTriangles, size_t aCount )
	{
		mTriangles.clear();
		mTriangles.reserve(aCount);
		mTriangles.insert(mTriangles.begin(), aTriangles, aTriangles+aCount);
	}

	void Mesh::SetNormals( const Vector* aNormals, size_t aCount )
	{
		mNormals.clear();
		mNormals.reserve(aCount);
		mNormals.insert(mNormals.begin(), aNormals, aNormals + aCount);
	}

	void Mesh::BuildNormals()
	{
		for(size_t i = 0; i < mTriangles.size(); ++i)
		{
			const Point& lA = mVertexs[ mTriangles[i].mVertex[0] ];
			const Point& lB = mVertexs[ mTriangles[i].mVertex[1] ];
			const Point& lC = mVertexs[ mTriangles[i].mVertex[2] ];
			Vector lNormal = CrossProduct( lB - lA, lC - lA );
			lNormal = Normalize(lNormal);
			mNormals.push_back(lNormal);
			mTriangles[i].mNormal = i;
			mTriangles[i].mVertexNormal[0] = mTriangles[i].mVertexNormal[1] = mTriangles[i].mVertexNormal[2] = i;
		}
	}

	bool Mesh::GetTriangleVertexs( int aTriIndex, Vector& aA, Vector& aB, Vector& aC) const
	{
		if(aTriIndex < 0 || aTriIndex >= (signed)mTriangles.size())
			return false;

		aA = mVertexs[mTriangles[aTriIndex].mVertex[0]];
		aB = mVertexs[mTriangles[aTriIndex].mVertex[1]];
		aC = mVertexs[mTriangles[aTriIndex].mVertex[2]];
	}
}
