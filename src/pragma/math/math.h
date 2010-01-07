#pragma once

#include <pragma/math/type_traits.h>

namespace pragma
{
	template <typename T>
	T abs(const T& aValue) { return (aValue < 0)? -aValue : aValue; }

	template <typename T>
	T min(const T& aValue1, const T& aValue2) { return (aValue1 < aValue2)? aValue1 : aValue2; }

	template <typename T>
	T max(const T& aValue1, const T& aValue2) { return (aValue1 > aValue2)? aValue1 : aValue2; }

	template <typename T>
	T clamp(const T& aValue, const T& aMin, const T& aMax) { return min( max(aValue, aMin), aMax); }

	template <typename T>
	bool near(T aValue1, T aValue2) { return abs<T>(aValue1 - aValue2) < math::type_traits<T>::epsilon; }

	template <typename T>
	T sqrt(const T& aValue);

	template <typename T>
	T Cot(const T& aValue);

	template <typename T>
	T Sin(const T& aValue);

	template <typename T>
	T Cos(const T& aValue);

}
