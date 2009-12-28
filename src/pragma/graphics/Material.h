#pragma once

#include <pragma/graphics/types.h>
#include <string>
#include <vector>

namespace pragma
{

	class Material
	{
	public:
							Material		(const char* aName);

		const std::string&	GetName			() const { return mName; }

		void				SetDiffuseColor	( const Color& aColor ) { mDiffuseColor = aColor; }
		const Color			GetdiffuseColor	() const { return mDiffuseColor; }

	private:
		std::string	mName;
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
