#pragma once

#include <string>

namespace pragma
{
	std::string GetPath(const char* aCompletePath);

	template <typename T>
	inline T Random();

	template <typename T>
	inline T Random(T aMin, T aMax);
}
