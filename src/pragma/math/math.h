#pragma once

namespace pragma
{
	template <typename T>
	T abs(const T& aValue) { return (aValue < 0)? -aValue : aValue; }

	template <typename T>
	T min(const T& aValue1, const T& aValue2) { return (aValue1 < aValue2)? aValue1 : aValue2; }

	template <typename T>
	T max(const T& aValue1, const T& aValue2) { return (aValue1 > aValue2)? aValue1 : aValue2; }

	template <typename T>
	T sqrt(const T& aValue);

	template <typename T>
	T Cot(const T& aValue);

}
