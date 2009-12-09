#include "type_traits.h"

namespace pragma { namespace math
{
	const float type_traits<float>::epsilon = 0.00001f;
	const double type_traits<double>::epsilon = 0.000001;
} }
