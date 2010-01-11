#include "system.h"

namespace pragma
{
	std::string GetPath(const char* aCompletePath)
	{
		std::string lRetVal = aCompletePath;
		while( lRetVal.length() &&
			   ( lRetVal[lRetVal.length()-1] != '\\' &&
			     lRetVal[lRetVal.length()-1] != '/' ) )
		{
			lRetVal.resize(lRetVal.length()-1);
		}

		return lRetVal;
	}

	template <>
	float Random()
	{
		return float(float(rand()) / RAND_MAX);
	}

	template <>
	double Random()
	{
		return double(double(rand()) / RAND_MAX);
	}

	template <>
	double Random(double aMin, double aMax)
	{
		return aMin + (Random<double>() * (aMax - aMin) );
	}

	template <>
	float Random(float aMin, float aMax)
	{
		return aMin + (Random<float>() * (aMax - aMin) );
	}

	template <>
	int Random(int aMin, int aMax)
	{
		return aMin + (rand() * (aMax - aMin) / RAND_MAX );
	}
}
