#include <pragma/graphics/types.h>
#include "Parsers.h"

#include <stdio.h>
#include <vector>
#include <string>

#include <pragma/graphics/Mesh.h>
#include <pragma/graphics/Material.h>
#include <pragma/system/system.h>

namespace pragma
{

	bool ParseMaterialsOBJ(const char* aFilename, MaterialLibrary& aMaterialLibrary)
	{
		FILE* handle = fopen(aFilename, "rb");

		if( handle == 0)
			return false;

		// Determinamos el tamaño del archivo
		fseek(handle, 0, SEEK_END);
		size_t lFileSize = ftell(handle);
		fseek(handle, 0, SEEK_SET);

		// Cargamos el contenido del archivo en un buffer de memoria
		char* lText = new char[lFileSize+1];
		fread(lText, 1, lFileSize, handle);
		lText[lFileSize] = 0;
		fclose(handle);

		// Tokenizamos todo el fichero y metemos los tokens en un vector de strings
		std::vector<std::string> lTokens;
		Tokenize(lText, lTokens);

		// Ya no necesitamos el buffer de texto
		delete[] lText;

		size_t lCount = 0;
		while(lCount < lTokens.size())
		{
			if(lTokens[lCount] == "newmtl")
			{
				std::string lName = lTokens[lCount+1];
				Material lMaterial(lName.c_str());
				Color lDiffuse;
				lDiffuse.i[0] = Parse<Real>(lTokens[lCount+21]);
				lDiffuse.i[1] = Parse<Real>(lTokens[lCount+22]);
				lDiffuse.i[2] = Parse<Real>(lTokens[lCount+23]);
				lMaterial.SetDiffuseColor(lDiffuse);
				aMaterialLibrary.AddMaterial(lMaterial);
				lCount+= 31;
			}
			else
				lCount++;
		}
		

		return true;
	}

	bool ParseOBJ(const char* aFilename, Mesh& aMesh, MaterialLibrary& aMaterialLibrary)
	{
		FILE* handle = fopen(aFilename, "rb");

		if( handle == 0)
			return false;

		// Determinamos el tamaño del archivo
		fseek(handle, 0, SEEK_END);
		size_t lFileSize = ftell(handle);
		fseek(handle, 0, SEEK_SET);

		// Cargamos el contenido del archivo en un buffer de memoria
		char* lText = new char[lFileSize+1];
		fread(lText, 1, lFileSize, handle);
		lText[lFileSize] = 0;
		fclose(handle);

		// Tokenizamos todo el fichero y metemos los tokens en un vector de strings
		std::vector<std::string> lTokens;
		Tokenize(lText, lTokens);

		// Ya no necesitamos el buffer de texto
		delete[] lText;

		//-- Parse materials file ----------------------------------------------
		size_t lPos = 0;
		while(lTokens[lPos] != "mtllib")
			lPos++;
		ParseMaterialsOBJ((GetPath(aFilename) + lTokens[lPos+1]).c_str(), aMaterialLibrary);

		//-- Parse vertex positions --------------------------------------------
		std::vector<Point> lPositions;
		lPos = 0;
		while(lTokens[lPos] != "v")
		{
			lPos++;
		}

		while(lTokens[lPos] == "v")
		{
			lPositions.push_back(Point());
			Point& lPoint = lPositions.back();
			lPoint.x = Parse<Real>(lTokens[lPos+1]);
			lPoint.y = Parse<Real>(lTokens[lPos+2]);
			lPoint.z = Parse<Real>(lTokens[lPos+3]);
			lPos+= 4;
		}
		aMesh.SetVertexs(&lPositions[0], lPositions.size());

		//-- Parse vertex normals ----------------------------------------------
		std::vector<Vector> lNormals;
		while(lTokens[lPos] != "vn")
			lPos++;
		while(lTokens[lPos] == "vn")
		{
			lNormals.push_back(Vector());
			Vector& lNormal = lNormals.back();
			lNormal.x = Parse<Real>(lTokens[lPos+1]);
			lNormal.y = Parse<Real>(lTokens[lPos+2]);
			lNormal.z = Parse<Real>(lTokens[lPos+3]);
			lPos+= 4;
		}
		aMesh.SetNormals(&lNormals[0], lNormals.size());

		while(lTokens[lPos] != "f")
		{
			lPos++;
		}

		//-- Parse Triangle indices --------------------------------------------
		int lCurrentMaterial = 0;
		std::vector<Mesh::TTriangle> lFaces;
		lPos = 0;
		while(true)
		{
			if(lPos >= lTokens.size())
				break;

			if(lTokens[lPos] == "usemtl")
			{
				lCurrentMaterial = aMaterialLibrary.GetMaterialIndex(lTokens[lPos+1].c_str());
				lPos+= 2;
			}

			if(lTokens[lPos] != "f")
			{
				lPos++;
				continue;
			}

			lFaces.push_back(Mesh::TTriangle());
			Mesh::TTriangle& lTri = lFaces.back();

			for(size_t i = 0; i < 3; ++i)
			{
				const char* lStr = lTokens[lPos+1+i].c_str();
				const char* lStrEnd = lStr;
				while(*lStrEnd != '/') lStrEnd++;
				std::string lNumer(lStr, lStrEnd);
				lTri.mVertex[i] = Parse<int>(lNumer)-1;

				lStr = lStrEnd+1;
				lStrEnd = lStr;
				while(*lStrEnd != '/') lStrEnd++;

				lStr = lStrEnd+1;
				lStrEnd = lStr;
				while(*lStrEnd != 0) lStrEnd++;
				lNumer = std::string(lStr, lStrEnd);
				lTri.mVertexNormal[i] = Parse<int>(lNumer)-1;
				lTri.mMaterial = lCurrentMaterial;
			}
			
			lPos+= 4;
		}

		aMesh.SetTriangles(&lFaces[0], lFaces.size());

		aMesh.BuildFaceNormals();

		return true;
	}
}
