#include "types.h"
#include "Mesh.h"

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
}
