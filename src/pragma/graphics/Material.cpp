#include "Material.h"

namespace pragma
{
	Material::Material(const char* aName, EType aType)
		: mName(aName)
		, mType(aType)
	{
	}

	// Material Library
	MaterialLibrary::MaterialLibrary( size_t aMaxMaterials )
		: mMaxMaterials(aMaxMaterials)
	{
		mMaterials.reserve(aMaxMaterials);
	}

	int MaterialLibrary::AddMaterial(const Material& aMaterial)
	{
		if(mMaterials.size() < mMaxMaterials)
		{
			mMaterials.push_back(aMaterial);
			return mMaterials.size()-1;
		}
		else
			return -1;
	}

	const Material& MaterialLibrary::GetMaterial(int aIndex) const
	{
		return mMaterials[aIndex];
	}

	int MaterialLibrary::GetMaterialIndex(const char* aName) const
	{
		for(size_t i = 0; i < mMaterials.size(); ++i)
		{
			if(mMaterials[i].GetName() == aName)
				return i;
		}
		return -1;
	}
}
