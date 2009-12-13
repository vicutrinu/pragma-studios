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

}
