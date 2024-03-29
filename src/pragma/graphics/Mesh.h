#pragma once

#include <pragma/graphics/types.h>
#include <string>
#include <vector>
#include <pragma/geometry/AABB.h>

namespace pragma
{
	class Mesh
	{
	public:
		typedef uint32 Index;

		struct TTriangle
		{
			Index	mVertex[3];
			Index	mVertexNormal[3];
			Index	mNormal;
			//Vector	mPlaneNormal;
			Index	mMaterial;
		};

	public:
							Mesh				() { }
							Mesh				( const char* aName );

		bool				IsOk				() const { return !mName.empty(); }

		const char* 		GetName				() const { return mName.c_str(); }
		size_t				GetNumVertexs		() const { return mVertexs.size(); }
		size_t				GetNumTriangles		() const { return mTriangles.size(); }

		const Point*		GetVertexs			( size_t& aCount ) const { aCount = mVertexs.size(); return &mVertexs[0]; }
		const TTriangle*	GetTriangles		( size_t& aCount ) const { aCount = mTriangles.size(); return &mTriangles[0]; }

		void				SetVertexs			( const Point* aVertexs, size_t aCount );
		void				SetTriangles		( const TTriangle* aTriangles, size_t aCount );
		void				SetNormals			( const Vector* aNormals, size_t aCount );

		void				BuildNormals		();
		void				BuildFaceNormals	();

		const Vector&		GetTriangleNormal	( int aTriIndex ) const { return mNormals[mTriangles[aTriIndex].mNormal]; }
		const Vector&		GetVertexNormal		( Index aNormalIndex ) const { return mNormals[aNormalIndex]; }

		TTriangle&			EditTriangle		( int aTriIndex ) { return mTriangles[aTriIndex]; }
		const TTriangle&	GetTriangle			( int aTriIndex ) const { return mTriangles[aTriIndex]; }

		bool				GetTriangleVertexs	( int aTriIndex, Vector& aA, Vector& aB, Vector& aC) const;
		/*
		void				AddVertexs		( const Point* aVertexs, size_t aCount );
		void				AddTriangles	( const TTriangle* aTriangles, size_t aCount );
		*/

		const AABB&			GetBoundingBox		() const { return mAABB; }

	private:
		std::string				mName;
		std::vector<Point>		mVertexs;
		std::vector<TTriangle>	mTriangles;
		std::vector<Vector>		mNormals;

		AABB					mAABB;
	};
}
