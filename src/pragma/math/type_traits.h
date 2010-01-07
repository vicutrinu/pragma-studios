#pragma once

namespace pragma { namespace math
{

	template<typename T>
	struct type_traits
	{
		static const T epsilon;
		static const T Pi;
		static const T DoublePi;
		static const T HalfPi;
	};

} }
