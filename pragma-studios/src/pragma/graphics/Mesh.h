#pragma once

#include <pragma/graphics/types.h>
#include <string>
#include <vector>

namespace pragma
{
	class Mesh
	{
	public:
		typedef uint32 Index;

		struct TTriangle
		{
			Index	mVertex[3];
			//Index	mVertexNormal[3];
			//Index	mNormal;
			//Vector	mPlaneNormal;
		};

	public:
							Mesh			() { }
							Mesh			( const char* aName );

		bool				IsOk			() const { return !mName.empty(); }

		const char* 		GetName			() const { return mName.c_str(); }
		size_t				GetNumVertexs	() const { return mVertexs.size(); }
		size_t				GetNumTriangles	() const { return mTriangles.size(); }

		const Point*		GetVertexs		(size_t& aCount) const { aCount = mVertexs.size(); return &mVertexs[0]; }
		const TTriangle*	GetTriangles	(size_t& aCount) const { aCount = mTriangles.size(); return &mTriangles[0]; }

		void				SetVertexs		( const Point* aVertexs, size_t aCount );
		void				SetTriangles	( const TTriangle* aTriangles, size_t aCount );

		/*
		void				AddVertexs		( const Point* aVertexs, size_t aCount );
		void				AddTriangles	( const TTriangle* aTriangles, size_t aCount );
		*/

	private:
		std::string				mName;
		std::vector<Point>		mVertexs;
		std::vector<TTriangle>	mTriangles;
	};
}
