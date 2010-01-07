#include <pragma/graphics/Mesh.h>
#include <pragma/graphics/import/Parsers.h>
#include <pragma/graphics/Camera.h>
#include <pragma/graphics/MeshCollision.h>

#include <pragma/math/types.h>
#include <pragma/math/functions.h>

#include <pragma/geometry/functions.h>

#include <stdio.h>

#include <pragma/system/clock.h>
#include <pragma/graphics/Material.h>
#include <pragma/image/Image.h>
#include <pragma/image/functions.h>
#include "Raytracer.h"
#include "Samplers.h"

using namespace pragma;

#define SAMPLES_PER_PIXEL	1
#define IMAGE_SIZE			512

int main(int argc, char* argv[])
{
	// Load ASE file
	pragma::Mesh lMesh;
	MaterialLibrary lMaterialLibrary(32);
	ParseOBJ("scenes/Cornell.obj", lMesh, lMaterialLibrary);
	TMillisecond lEnd = GetTimeMilliseconds();

	// Create camera
	Vector lCameraPos(0,27.5,95);
	Camera lCamera;
	lCamera.SetProjection(math::type_traits<Real>::DoublePi * 45 / 360, 1, 1, 150);
	lCamera.SetTransform( lCameraPos, Vector(0,27.5,0), Vector(0,1,0) );

	// Get camera inverse tranfor (from 2D to 3D world)
	matrix4x4f lTransform = lCamera.GetProjection() * lCamera.GetTransform();
	lTransform = Inverse(lTransform);

	Image lImage(IMAGE_SIZE, IMAGE_SIZE);
	RGBPixel* lIter = lImage.begin();

	Image lDebugImage(IMAGE_SIZE, IMAGE_SIZE);
	RGBPixel* lDebugIter = lDebugImage.begin();

	BasicSampler lSampler(SAMPLES_PER_PIXEL);

	Raytracer lRaytracer( lMesh, Point(0,55,0)
						, lMaterialLibrary
						, 1, 10 );

	int lProgess = 0;

	// Scanline renderer
	for(size_t i = 0; i < IMAGE_SIZE; ++i)
	{
		for(size_t j = 0; j < IMAGE_SIZE; ++j)
		{
			*lIter = Color(0,0,0);
			// Multiple samples per pixel
			for(size_t s = 0; s < lSampler.GetSamplesPerPixel(); ++s)
			{
				vector2f lOffset = lSampler.GetSampleOffset(s);
				vector4f lRay((((lOffset.x+j)*2.f)/(IMAGE_SIZE-1))-1, 1-(((lOffset.y+i)*2.f)/(IMAGE_SIZE-1)), 1, 1);
				vector4f lRes = TransformPoint(lTransform, lRay);
				lRes = lRes * (1.f / lRes.w);

				Vector lDir(lRes.x, lRes.y, lRes.z);
				lDir = lDir - lCameraPos;
				lDir = Normalize(lDir);

				*lIter = *lIter + RGBPixel(lRaytracer.TraceCameraRay( lCameraPos, lDir, 1000 ));
				*lDebugIter = RGBPixel(lRaytracer.Debug());
			}
			*lIter = *lIter / lSampler.GetSamplesPerPixel();
			++lIter;
			++lDebugIter;

			int lNewProgress = (i * IMAGE_SIZE + j) * 100 / (IMAGE_SIZE * IMAGE_SIZE);
			if(lProgess != lNewProgress)
			{
				printf("%3d%%\b\b\b\b", lNewProgress);
				lProgess = lNewProgress;
			}
		}
	}

	ExportToTGA(lImage, "out.tga");
	ExportToTGA(lDebugImage, "debug.tga");

	return 0;
}