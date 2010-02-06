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
		float lRetVal;
#pragma omp critical
		lRetVal = float(float(rand()) / RAND_MAX);
		return lRetVal;
	}

	template <>
	double Random()
	{
		float lRetVal;
#pragma omp critical
		lRetVal =  double(double(rand()) / RAND_MAX);
		return lRetVal;
	}

	template <>
	double Random(double aMin, double aMax)
	{
		float lRetVal;
#pragma omp critical
		lRetVal = aMin + (Random<double>() * (aMax - aMin) );
		return lRetVal;
	}

	template <>
	float Random(float aMin, float aMax)
	{
		float lRetVal;
#pragma omp critical
		lRetVal = aMin + (Random<float>() * (aMax - aMin) );
		return lRetVal;
	}

	template <>
	int Random(int aMin, int aMax)
	{
		float lRetVal;
#pragma omp critical
		lRetVal = aMin + (rand() * (aMax - aMin) / RAND_MAX );
		return lRetVal;
	}
}
