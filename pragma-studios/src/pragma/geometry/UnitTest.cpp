#include <pragma/geometry/types.h>
#include <pragma/geometry/intersection/ray_triangle.h>
#include <pragma/debug/debug.h>

namespace pragma
{

	bool DoIntersectionTests()
	{
		vector3f lA(0,0,0);
		vector3f lB(0,1,0);
		vector3f lC(1,0,0);

		vector2f lOut;
		float lDistanceOut;

		bool lRetVal = true;

		if(lRetVal)
		{
			vector3f lOrigin(0.5f,0.5f,-1);
			vector3f lDir(0,0,1);
			lRetVal = IntersectRayTriangle(lA, lB, lC, lOrigin, lDir, lOut, lDistanceOut);
			pragma_error_if(!lRetVal, "IntersectRayTriangle is not working properly");
		}

		if(lRetVal)
		{
			vector3f lOrigin(0.0001f,0,2);
			vector3f lDir(0,0,-1);
			IntersectRayTriangle(lA, lB, lC, lOrigin, lDir, float(1), lOut, lDistanceOut);
			IntersectRayTriangle(lA, lB, lC, lOrigin, lDir);
			IntersectRayTriangle_2Sided(lA, lB, lC, lOrigin, lDir, lOut, lDistanceOut);
			IntersectRayTriangle_2Sided(lA, lB, lC, lOrigin, lDir, float(1), lOut, lDistanceOut);
		}

		return lRetVal;
	}

}
