#pragma once

namespace pragma
{
	class Image;

	bool ExportToTGA	(const Image& aImage, const char* aFilename);
	bool ImportFromTGA	(Image& aImage, const char* aFilename);

}
