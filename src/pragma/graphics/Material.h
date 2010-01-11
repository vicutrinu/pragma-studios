#pragma once

#include <pragma/graphics/types.h>
#include <string>
#include <vector>

namespace pragma
{

	class Material
	{
	public:
		enum EType
		{
			eSolid = 0,
			eReflect,
		};

	public:
							Material		(const char* aName, EType aType);

		const std::string&	GetName			() const { return mName; }

		void				SetDiffuseColor	( const Color& aColor ) { mDiffuseColor = aColor; }
		const Color			GetdiffuseColor	() const { return mDiffuseColor; }

		EType				GetType			() const { return mType; }

	private:
		std::string	mName;
		EType		mType;

		Color		mDiffuseColor;
	};

	class MaterialLibrary
	{
	public:
						MaterialLibrary	( size_t aMaxMaterials );

		int				AddMaterial		( const Material& aMaterial );
		const Material& GetMaterial		( int aIndex ) const;
		int				GetMaterialIndex( const char* aName ) const;

	private:
		size_t				  mMaxMaterials;
		std::vector<Material> mMaterials;
	};

}
