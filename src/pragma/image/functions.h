#pragma once

#include <pragma/types.h>

namespace pragma
{
	class Image;

	bool ExportToTGA	(const Image& aImage, const char* aFilename);
	bool ExportToTGA	(const uint8* aImage, size_t aWidth, size_t aHeight, const char* aFilename);
	bool ImportFromTGA	(Image& aImage, const char* aFilename);

}
