#include <pragma/system/clock.h>

#include <stdio.h>
#include <time.h>

namespace pragma
{
	TMillisecond GetTimeMilliseconds()
	{
		return (clock() * 1000 / CLOCKS_PER_SEC);
	}
}
