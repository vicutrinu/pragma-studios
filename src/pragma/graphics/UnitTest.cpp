#include "types.h"
#include "Camera.h"

namespace pragma
{
	bool DoGraphicsTests()
	{
		Camera lCamera;
		lCamera.SetTransform(Point(0,0,0), Vector(0,0,-1), Vector(0,1,0));
		//lCamera.GetTransform() * Point(0,0,-2);

		return true;
	}
}
