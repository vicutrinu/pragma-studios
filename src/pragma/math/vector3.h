#pragma once

#include "vector2.h"

namespace pragma
{

	/**
	 *	base_vector3 debe ser un tipo sin constructores. De esta manera podemos usar este tipo
	 *	dentro de uniones anonimas.
	 */
	template <typename T>
	struct base_vector3
	{
		union
		{
			T i[3];
			struct
			{
				T x;
				T y;
				T z;
			};
		};		
	};

	/**
	 *	Esta clase unicamente proporciona constructores para objetos del tipo base_vector3
	 */
	template <typename T>
	struct vector3 : public base_vector3<T>
	{
		vector3 () { }
		
		vector3 (const base_vector3<T>& aVector)
		{
			*(base_vector3*)this = aVector;
		}

		vector3 (const T& aX, const T& aY, const T& aZ)
		{
			base_vector3<T>::x = aX;
			base_vector3<T>::y = aY;
			base_vector3<T>::z = aZ;
		}

		vector3 (const vector2<T>& aVector2, const T& aValue)
		{
			base_vector3<T>::x = aVector2.x;
			base_vector3<T>::y = aVector2.y;
			base_vector3<T>::z = aValue;
		}

		template<typename T2>
		vector3 (const base_vector3<T2>& aVector)
		{
			// Algun type_trait deberia decirme si se puede hacer la conversion de forma segura
			base_vector3<T>::x = T(aVector.x);
			base_vector3<T>::y = T(aVector.y);
			base_vector3<T>::z = T(aVector.z);
		}

	};
};