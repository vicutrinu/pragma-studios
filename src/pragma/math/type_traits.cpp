#include "type_traits.h"

namespace pragma { namespace math
{
	template<>
	const float type_traits<float>::epsilon = 0.00001f;
	
	template<>
	const double type_traits<double>::epsilon = 0.00000001;

	template<>
	const float type_traits<float>::Pi = 3.14159265358979323846264338327950288419716939937510f;

	template<>
	const double type_traits<double>::Pi = 3.14159265358979323846264338327950288419716939937510;

	template<>
	const float type_traits<float>::DoublePi = 2 * Pi;

	template<>
	const double type_traits<double>::DoublePi = 2 * Pi;
} }
