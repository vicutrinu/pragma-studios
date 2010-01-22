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
		
		const vector2f GetSample(size_t aIndex) const { return mValues[aIndex]; }

	private:
		size_t mSamplesPerPixel;
		std::vector<vector2f> mValues;
	};

	Vector UniformSampleHemisphere(Real u1, Real u2)
	{
		Real lPhi = math::type_traits<Real>::DoublePi * u1;
		Real lR = sqrt<Real>(u2);
		return Vector( lR * Cos<Real>(lPhi)
					 , sqrt<Real>(1 - u2)
					 , lR * Sin<Real>(lPhi) );

		/*Real lR = sqrt<Real>(max(Real(0), Real(1) - u1 * u1));
		Real lPhi = math::type_traits<Real>::DoublePi * u2;
		return Vector( lR * Cos<Real>(lPhi)
					 , u1
					 , lR * Sin<Real>(lPhi) );*/
	}

	Vector UniformSampleHemisphere(Real u1, Real u2, const Vector& aNormal)
	{
		Real r1 = math::type_traits<Real>::DoublePi * u1;
		Real r2 = u2;
		Real r2s = sqrt<Real>(r2);

		Vector w = aNormal;
		Vector u = Normalize(((abs(w.x)>.1 ? Vector(0,1,0) : CrossProduct(Vector(1,0,0),w))));
		Vector v = CrossProduct(w,u); 
		Vector d = Normalize((u*Cos(r1)*r2s + v*Sin(r1)*r2s + w*sqrt(1-r2))); 
		return d; 
	}
}
