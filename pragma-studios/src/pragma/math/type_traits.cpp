#include "type_traits.h"

namespace pragma { namespace math
{
	template<>
	const float type_traits<float>::epsilon = 0.00001f;
	
	template<>
	const double type_traits<double>::epsilon = 0.000001;
} }
