#pragma once

namespace pragma
{

	/**
	 *	base_vector2 debe ser un tipo sin constructores. De esta manera podemos usar este tipo
	 *	dentro de uniones anonimas.
	 */
	template <typename T>
	struct base_vector2
	{
		union 
		{
			T i[2];
			struct
			{
				T x;
				T y;
			};
		};
	};

	/**
	 *	Esta clase unicamente proporciona constructores para objetos del tipo base_vector2
	 */
	template <typename T>
	struct vector2 : public base_vector2<T>
	{
		vector2() { }
		
		vector2(const base_vector2<T>& aVector)
		{
			*(base_vector2<T>*)this = aVector;
		}

		vector2 (const T& aX, const T& aY)
		{
			base_vector2<T>::x = aX;
			base_vector2<T>::y = aY;
		}
	};

};