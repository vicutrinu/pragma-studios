#include "Parsers.h"

#include <string>
#include <vector>

namespace pragma
{

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

	template<>
	double Parse<double>(const std::string& aString)
	{
		return (double)atof(aString.c_str());
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
	void Tokenize(const char* aTextBuffer, std::vector<std::string>& aTokens)
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

}
