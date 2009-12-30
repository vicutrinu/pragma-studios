#pragma once

#include "vector2.h"

namespace pragma
{

	/**
	 *	base_vector4 debe ser un tipo sin constructores. De esta manera podemos usar este tipo
	 *	dentro de uniones anonimas.
	 */
	template <typename T>
	struct base_vector4
	{
		union
		{
			T i[4];
			struct
			{
				T x;
				T y;
				T z;
				T w;
			};
		};		
	};

	/**
	 *	Esta clase unicamente proporciona constructores para objetos del tipo base_vector4
	 */
	template <typename T>
	struct vector4 : public base_vector4<T>
	{
		vector4 () { }
		
		vector4(const base_vector4<T>& aVector)
		{
			*((base_vector4<T>*)this) = aVector;
		}

		vector4 (const T& aX, const T& aY, const T& aZ, const T& aW)
		{
			base_vector4<T>::x = aX;
			base_vector4<T>::y = aY;
			base_vector4<T>::z = aZ;
			base_vector4<T>::w = aW;
		}

		vector4 (const vector3<T>& aVector3, const T& aValue)
		{
			base_vector4<T>::x = aVector3.x;
			base_vector4<T>::y = aVector3.y;
			base_vector4<T>::z = aVector3.z;
			base_vector4<T>::w = aValue;
		}

	};
};