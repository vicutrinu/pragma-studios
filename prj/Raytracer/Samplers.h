#pragma once

namespace pragma
{
	class BasicSampler
	{
public:
		BasicSampler(size_t aSamplesPerPixel)
			: mSamplesPerPixel(aSamplesPerPixel)
		{
			mValues.reserve(mSamplesPerPixel);
			size_t lWidth = (size_t)pragma::sqrt<float>((float)mSamplesPerPixel);
			size_t lHeigth = mSamplesPerPixel / lWidth;
			vector2f lOffset( 1.f / lWidth, 1.f / lHeigth);
			vector2f lSample = lOffset / 2;
			for(size_t i = 0; i < lHeigth; ++i)
			{
				for(size_t j = 0; j < lWidth; ++j)
				{
					mValues.push_back(lSample);										
					lSample.x+= lOffset.x;
				}
				lSample.y+= lOffset.y;
			}
		}

		size_t GetSamplesPerPixel() const { return mSamplesPerPixel; }
		const vector2f GetSampleOffset(size_t aIndex) const
		{
			return mValues[aIndex];
			
		}

	private:
		size_t mSamplesPerPixel;
		std::vector<vector2f> mValues;
	};
}
