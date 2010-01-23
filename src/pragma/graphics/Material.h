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
			eRefract,
		};

	public:
							Material		(const char* aName, EType aType);

		const std::string&	GetName			() const { return mName; }

		bool				HasDiffuse		() const;
		void				SetDiffuseColor	( const Color& aColor ) { mDiffuseColor = aColor; }
		const Color			GetdiffuseColor	() const { return mDiffuseColor; }

		bool				HasSpecular		() const;
		void				SetSpecularColor( const Color& aColor ) { mSpecularColor = aColor; }
		const Color			GetSpecularColor() const { return mSpecularColor; }

		EType				GetType			() const { return mType; }

	private:
		std::string	mName;
		EType		mType;

		Color		mDiffuseColor;
		Color		mSpecularColor;
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
