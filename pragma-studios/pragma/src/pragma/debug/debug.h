#pragma once

#include <stdio.h>

namespace pragma
{

#define pragma_assert(expr, msg, ...) pragma_error_if(!expr, "%s\n", msg, __VA_ARGS__)
#define pragma_error_if(expr, msg, ...) if(expr) printf( "%s\n", msg, __VA_ARGS__)
#define pragma_warning_if(expr, msg, ...) if(expr) printf( "%s\n", msg, __VA_ARGS__)

}
