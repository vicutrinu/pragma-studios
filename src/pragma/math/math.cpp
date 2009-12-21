#include "math.h"
#include <math.h>

namespace pragma
{

	template <>
	float sqrt(const float& aValue)
	{
		return ::sqrt(aValue);
	}

	template <>
	float Cot(const float& aValue)
	{
		return 1.f / ::tan(aValue);
	}

	template <>
	double sqrt(const double& aValue)
	{
		return ::sqrt(aValue);
	}

	template <>
	double Cot(const double & aValue)
	{
		return double(1) / ::tan(aValue);
	}

}
