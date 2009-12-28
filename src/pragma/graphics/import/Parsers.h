#pragma once

#include <string>
#include <vector>

namespace pragma
{
	class Mesh;
	class MaterialLibrary;

	// Parseo de elementos base (enteros, flotantes, strings...)
	template<typename T>
	T Parse(const std::string& aString);

	void Tokenize(const char* aTextBuffer, std::vector<std::string>& aTokens);

	bool ParseASE(const char* aFilename, Mesh& aMesh);
	bool ParseOBJ(const char* aFilename, Mesh& aMesh, MaterialLibrary& aMaterialLibrary);
}
