#include <pragma/system/clock.h>

#include <stdio.h>
#include <time.h>

namespace pragma
{
	TMillisecond GetTimeMilliseconds()
	{
		return ((float)clock() * 1000.f / CLOCKS_PER_SEC);
	}
}
