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

	template <>
	float Sin(const float& aValue)
	{
		return float(::sin(aValue));
	}

	template <>
	float Cos(const float& aValue)
	{
		return float(::cos(aValue));
	}

	template <>
	double Sin(const double& aValue)
	{
		return double(::sin(aValue));
	}

	template <>
	double Cos(const double& aValue)
	{
		return double(::cos(aValue));
	}

	template <>
	float Asin(const float& aValue)
	{
		return float(::asin(aValue));
	}

	template <>
	double Asin(const double& aValue)
	{
		return double(::asin(aValue));
	}

	template <>
	float Acos(const float& aValue)
	{
		return float(::acos(aValue));
	}

	template <>
	double Acos(const double& aValue)
	{
		return double(::acos(aValue));
	}

}
