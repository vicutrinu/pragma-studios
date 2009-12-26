#include <pragma/graphics/types.h>
#include "ASE_Parser.h"

#include <stdio.h>
#include <vector>
#include <string>

#include <pragma/graphics/Mesh.h>

namespace pragma
{

	// Parseo de elementos base (enteros, flotantes, strings...)
	template<typename T>
	T Parse(const std::string& aString);

	template<>
	int Parse<int>(const std::string& aString)
	{
		return atoi(aString.c_str());
	}

	template<>
	float Parse<float>(const std::string& aString)
	{
		return (float)atof(aString.c_str());
	}

	/**
	 *	Dado un grupo de tokens, busca una clave en el y retorna el siguiente elemento (que debe ser una seccion seccion)
	 *	Si el siguiente elemento no es un comienzo de seccion o no encuentra la clave en el grupo, retorna false
	 *	Sino retorna la seccion que sigue al token aKey, sin las llaves que la encapsulan
	 */
	static bool GetSection(const char* aKey, const std::vector<std::string>& aTokens, std::vector<std::string>& aSection)
	{
		aSection.clear();
		std::vector<std::string>::const_iterator lIter = aTokens.begin();
		while(lIter != aTokens.end())
		{
			if(strcmp(aKey, (*lIter).c_str()) == 0)
			{
				++lIter;
				if(lIter == aTokens.end())
					return false;
				if(strcmp((*lIter).c_str(), "{") == 0)
				{
					++lIter;
					size_t lStartSections = 1;
					while(lStartSections != 0)
					{
						if(lIter == aTokens.end())
							return false;
						aSection.push_back(*lIter);
						if(strcmp((*lIter).c_str(), "{") == 0)
							lStartSections++;
						if(strcmp((*lIter).c_str(), "}") == 0)
							lStartSections--;
						++lIter;
					}
					aSection.pop_back();
					return true;
				}
				else
				{
					aSection.push_back(*lIter);
					return true;
				}
			}
			++lIter;
		}
		return false;
	}

	/**
	 *	Dado un grupo de tokens, busca una clave en el y retorna el siguiente elemento
	 *	Si el siguiente elemento es un comienzo de seccion o no encuentra la clave en el grupo, retorna
	 *	una string vacia, sino retorna una string conteniendo el token siguiente a la clave
	 */
	static std::string GetSection(const char* aKey, const std::vector<std::string>& aTokens)
	{
		std::vector<std::string> lSection;
		GetSection(aKey, aTokens, lSection);
		if(lSection.size() == 1)
			return lSection[0];
		else
			return std::string();
	}

	// Determina si un caracter es un separador o no
	static inline bool IsSeparator(char aCharacter)
	{
		switch(aCharacter)
		{
		case ' ':
		case '\n':
		case '\r':
		case '\t':
			return true;
		default:
			return false;
		}
	}

	/**
	 *	Separa un texto en tokens. La funcion IsSeparator decide que caracteres son los que separan 
	 *	un token del siguiente
	 *	Se considera el caso especial de un token formado por el caracter comillas seguido
	 *	de varios caracteres de cualquier tipo y acabado en el caracter comillas
	 */
	static void Tokenize(const char* aTextBuffer, std::vector<std::string>& aTokens)
	{
		aTokens.clear();
		while(true)
		{
			std::string lString;
			while(IsSeparator(*aTextBuffer))
			{	// Saltamos los espacios del principio
				aTextBuffer++;
				if(*aTextBuffer == 0)
					return;
			}
			if(*aTextBuffer == '\"')
			{	// parseamos una string tal cual, hasta que encontremos unas comillas
				aTextBuffer++;
				while(*aTextBuffer != '\"')
				{
					if(*aTextBuffer == 0)
						return;
					lString.push_back(*aTextBuffer);
					aTextBuffer++;
				}
				aTextBuffer++;
				aTokens.push_back(lString);
			}
			else
			{
				while(!IsSeparator(*aTextBuffer))
				{
					lString.push_back(*aTextBuffer);
					aTextBuffer++;
					if(*aTextBuffer == 0)
						break;
				}
				aTokens.push_back(lString);
			}
		}
	}

	bool ParseASE(const char* aFilename, Mesh& aMesh)
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

		std::string lString;

		std::vector<std::string> lObjectSection;
		GetSection("*GEOMOBJECT", lTokens, lObjectSection);
		std::string lName = GetSection("*NODE_NAME", lObjectSection);
		if(!lName.empty())
		{
			aMesh = Mesh(lName.c_str());
		}

		std::vector<std::string> lMeshSection;
		if(!GetSection("*MESH", lObjectSection, lMeshSection))
			return false;

		lString = GetSection("*MESH_NUMVERTEX", lMeshSection);
		if(lString.empty())
			return false;
		int lVertexCount = Parse<int>(lString);
		
		std::vector<std::string> lVertexListSection;
		if(!GetSection("*MESH_VERTEX_LIST", lMeshSection, lVertexListSection))
			return false;

		if(lVertexCount * 5 != lVertexListSection.size())
			return false;

		std::vector<std::string>::iterator lIter = lVertexListSection.begin();
		std::vector<Point> lPositions;
		lPositions.reserve(lVertexCount);
		while(lIter != lVertexListSection.end())
		{
			Point lVector( Parse<float>(lIter[2])
						  , Parse<float>(lIter[3])
						  , Parse<float>(lIter[4]) );
			lPositions.push_back(lVector);
			lIter+= 5;
		}

		aMesh.SetVertexs(&lPositions[0], lPositions.size());

		lString = GetSection("*MESH_NUMFACES", lMeshSection);
		if(lString.empty())
			return false;
		int lFaceCount = Parse<int>(lString);

		std::vector<std::string> lFaceListSection;
		if(!GetSection("*MESH_FACE_LIST", lMeshSection, lFaceListSection))
			return false;

		if(lFaceCount * 18 != lFaceListSection.size())
			return false;

		lIter = lFaceListSection.begin();
		std::vector<Mesh::TTriangle> lFaces;
		lFaces.reserve(lFaceCount);
		while(lIter != lFaceListSection.end())
		{
			lFaces.push_back(Mesh::TTriangle());
			Mesh::TTriangle& lFace = lFaces.back();
			lFace.mVertex[0] = Parse<int>(lIter[3]);
			lFace.mVertex[1] = Parse<int>(lIter[5]);
			lFace.mVertex[2] = Parse<int>(lIter[7]);
			lFace.mNormal = -1;
			lFace.mVertexNormal[0] = lFace.mVertexNormal[1] = lFace.mVertexNormal[2] = -1;
			/*
			lFace.mSmoothingID = Parse<int>(lIter[15]);
			lFace.mMaterialID = Parse<int>(lIter[17]);
			*/
			
			lIter+= 18;
		}

		aMesh.SetTriangles(&lFaces[0], lFaces.size());

		bool lBuildNormals = true;

		std::vector<std::string> lNormalsSection;
		std::vector<Vector> lNormals;
		int lCount = 0;
		if(GetSection("*MESH_NORMALS", lObjectSection, lNormalsSection))
		{
			lIter = lNormalsSection.begin();
			while(lIter != lNormalsSection.end())
			{
				Mesh::TTriangle& lTri = aMesh.EditTriangle(lCount);
				Vector lVector( Parse<float>(lIter[2]), Parse<float>(lIter[3]), Parse<float>(lIter[4]) );
				lNormals.push_back(lVector); // Face Normal
				lVector = Vector( Parse<float>(lIter[7]), Parse<float>(lIter[8]), Parse<float>(lIter[9]) );
				lNormals.push_back(lVector); // Vertex Normal
				lVector = Vector( Parse<float>(lIter[12]), Parse<float>(lIter[13]), Parse<float>(lIter[14]) );
				lNormals.push_back(lVector); // Vertex Normal
				lVector = Vector( Parse<float>(lIter[17]), Parse<float>(lIter[18]), Parse<float>(lIter[19]) );
				lNormals.push_back(lVector); // Vertex Normal
				lTri.mNormal = lCount * 4;
				lTri.mVertexNormal[0] = lCount * 4 + 1;
				lTri.mVertexNormal[1] = lCount * 4 + 2;
				lTri.mVertexNormal[2] = lCount * 4 + 3;

				lCount++;				
				lIter+= 20;
			}
			aMesh.SetNormals(&lNormals[0], lNormals.size());
			lBuildNormals = false;
		}

		if(lBuildNormals)
			aMesh.BuildNormals();

		return true;
	}
}
