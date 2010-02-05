#include <pragma/graphics/Mesh.h>
#include <pragma/graphics/import/Parsers.h>
#include <pragma/graphics/Camera.h>
#include <pragma/graphics/Material.h>

#include <pragma/image/Image.h>
#include <pragma/image/functions.h>

#include "Raytracer.h"

#include <stdio.h>

using namespace pragma;

#define IMAGE_SIZE			512

int main(int argc, char* argv[])
{
	// Load ASE file
	pragma::Mesh lMesh;
	MaterialLibrary lMaterialLibrary(32);
	ParseOBJ("scenes/Cornell.obj", lMesh, lMaterialLibrary);

	// Create camera
	Vector lCameraPos(0,27.5,95);
	Camera lCamera;
	lCamera.SetProjection(math::type_traits<Real>::DoublePi * 45 / 360, 1, 1, 150);
	lCamera.SetTransform( lCameraPos, Vector(0,27.5,0), Vector(0,1,0) );

	Image lImage(IMAGE_SIZE, IMAGE_SIZE);
	
	Raytracer lRaytracer( lMesh, Point(0,55,0), lMaterialLibrary, 1, 1, 10 );

	lRaytracer.Render( lCamera, lImage );

	ExportToTGA(lImage, "out.tga");

	return 0;
}