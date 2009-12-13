#include "types.h"
#include "math.h"
#include "functions.h"
#include "pragma/debug/debug.h"

namespace pragma
{
	bool Vector2fTests()
	{
		bool lRetVal = true;
		vector2f lVector1(1,2);
		vector2f lVector2;
		lVector2.x = 1;
		lVector2.y = 2;

		lRetVal = lVector1 == lVector2;
		pragma_error_if(!lRetVal, "UnitTest: operator== for vector2f is not working properly");

		if(lRetVal)
			lVector2 = vector2f(4,5);

		vector2f lVector3;

		if(lRetVal)
		{
			lVector3 = lVector1 + lVector2;
			lRetVal = lVector3 == vector2f(5, 7);
			pragma_error_if(!lRetVal, "UnitTest: operator+ for vector2f is not working properly");
		}

		if(lRetVal)
		{
			lVector3 = lVector2 - vector2f(4,5);
			lRetVal = lVector3 == vector2f(0,0);
			pragma_error_if(!lRetVal, "UnitTest: operator- for vector2f is not working properly");
		}

		return lRetVal;
	}

	bool Vector3fTests()
	{
		bool lRetVal = true;
		vector3f lVector1(1,2,3);
		vector3f lVector2;
		lVector2.x = 1;
		lVector2.y = 2;
		lVector2.z = 3;

		lRetVal = lVector1 == lVector2;
		pragma_error_if(!lRetVal, "UnitTest: operator== for vector3f is not working properly");

		if(lRetVal)
			lVector2 = vector3f(4,5,6);

		vector3f lVector3;

		if(lRetVal)
		{
			lVector3 = lVector1 + lVector2;
			lRetVal = lVector3 == vector3f(5, 7, 9);
			pragma_error_if(!lRetVal, "UnitTest: operator+ for vector3f is not working properly");
		}

		if(lRetVal)
		{
			lVector3 = lVector2 - vector3f(4,5,6);
			lRetVal = lVector3 == vector3f(0,0,0);
			pragma_error_if(!lRetVal, "UnitTest: operator- for vector3f is not working properly");
		}

		if(lRetVal)
		{
			lVector1 = vector3f(2,3,4);
			lVector2 = vector3f(5,6,7);
			lRetVal = DotProduct(lVector1, lVector2) == (10 + 18 + 28);
			pragma_error_if(!lRetVal, "UnitTest: operation DotProduct is not working properly");
		}

		if(lRetVal)
		{
			lVector1 = vector3f(1,2,3);
			lVector2 = vector3f(4,5,6);
			lRetVal = CrossProduct(lVector1, lVector2) == vector3f(-3,6,-3);
			pragma_error_if(!lRetVal, "UnitTest: operation CrossProduct is not working properly");
		}

		return lRetVal;
	}

	bool Vector4fTests()
	{
		bool lRetVal = true;
		vector4f lVector1(1,2,3,4);
		vector4f lVector2;
		lVector2.x = 1;
		lVector2.y = 2;
		lVector2.z = 3;
		lVector2.w = 4;

		lRetVal = lVector1 == lVector2;
		pragma_error_if(!lRetVal, "UnitTest: operator== for vector4f is not working properly");

		if(lRetVal)
			lVector2 = vector4f(4,5,6,7);

		vector4f lVector3;

		if(lRetVal)
		{
			lVector3 = lVector1 + lVector2;
			lRetVal = lVector3 == vector4f(5, 7, 9, 11);
			pragma_error_if(!lRetVal, "UnitTest: operator+ for vector4f is not working properly");
		}

		if(lRetVal)
		{
			lVector3 = lVector2 - vector4f(4,5,6,7);
			lRetVal = lVector3 == vector4f(0,0,0,0);
			pragma_error_if(!lRetVal, "UnitTest: operator- for vector4f is not working properly");
		}

		return lRetVal;
	}

	bool Matrix3fTests()
	{
		matrix3x3f lMatrix(0, 1, 2, 3, 4, 5, 6, 7, 8);

		bool lRetVal = true;

		lRetVal = lMatrix.i[0][0] == 0 && lMatrix.i[0][1] == 1 && lMatrix.i[0][2] == 2 &&
				  lMatrix.i[1][0] == 3 && lMatrix.i[1][1] == 4 && lMatrix.i[1][2] == 5 &&
				  lMatrix.i[2][0] == 6 && lMatrix.i[2][1] == 7 && lMatrix.i[2][2] == 8;

		pragma_error_if(!lRetVal, "UnitTest: matrix3x3 is not working properly. Check construction");

		lRetVal = lRetVal && lMatrix.mRow0 == vector3f(0,1,2) &&
							 lMatrix.mRow1 == vector3f(3,4,5) &&
							 lMatrix.mRow2 == vector3f(6,7,8);

		pragma_error_if(!lRetVal, "UnitTest: matrix3x3 is not working properly. Check construction");

		lRetVal = lRetVal && (sizeof(matrix3x3f) == sizeof(float) * (3*3));

		pragma_error_if(!lRetVal, "UnitTest: matrix3x3 is not working properly. Check size");

		matrix3x3f lAdd = lMatrix + lMatrix;
		lMatrix = lMatrix * 2.f;
		lRetVal = lRetVal && (lAdd == lMatrix );
		pragma_error_if(!lRetVal, "UnitTest: matrix3x3 is not working properly. Check size");

		return lRetVal;
	}

	bool Matrix4fTests()
	{
		matrix4x4f lMatrix(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

		bool lRetVal = true;

		lRetVal = lMatrix.i[0][0] ==  0 && lMatrix.i[0][1] ==  1 && lMatrix.i[0][2] ==  2 && lMatrix.i[0][3] ==  3 &&
				  lMatrix.i[1][0] ==  4 && lMatrix.i[1][1] ==  5 && lMatrix.i[1][2] ==  6 && lMatrix.i[1][3] ==  7 &&
				  lMatrix.i[2][0] ==  8 && lMatrix.i[2][1] ==  9 && lMatrix.i[2][2] == 10 && lMatrix.i[2][3] == 11 &&
				  lMatrix.i[3][0] == 12 && lMatrix.i[3][1] == 13 && lMatrix.i[3][2] == 14 && lMatrix.i[3][3] == 15 ;

		pragma_error_if(!lRetVal, "UnitTest: matrix4x4 is not working properly. Check construction");

		lRetVal = lRetVal && lMatrix.mRow0 == vector4f(0,1,2,3) &&
							 lMatrix.mRow1 == vector4f(4,5,6,7) &&
							 lMatrix.mRow2 == vector4f(8,9,10,11) && 
							 lMatrix.mRow3 == vector4f(12,13,14,15);

		pragma_error_if(!lRetVal, "UnitTest: matrix4x4 is not working properly. Check construction");

		lRetVal = lRetVal && (sizeof(matrix4x4f) == sizeof(float) * (4*4));

		pragma_error_if(!lRetVal, "UnitTest: matrix4x4 is not working properly. Check size");

		lMatrix = matrix4x4f( 1, 2,-1, 2
							, 2, 2,-1, 1
							,-1,-1, 1,-1
							, 2, 1,-1, 2);

		matrix4x4f lInverse = Inverse(lMatrix);
		matrix4x4f lIdentity = lMatrix * lInverse;
		lRetVal = lRetVal &&
				  abs<float>(1 - lIdentity.i[0][0]) < math::type_traits<float>::epsilon &&
				  abs<float>(1 - lIdentity.i[1][1]) < math::type_traits<float>::epsilon &&
				  abs<float>(1 - lIdentity.i[2][2]) < math::type_traits<float>::epsilon &&
				  abs<float>(1 - lIdentity.i[3][3]) < math::type_traits<float>::epsilon &&
				  abs<float>(    lIdentity.i[0][1]) < math::type_traits<float>::epsilon &&
				  abs<float>(    lIdentity.i[0][2]) < math::type_traits<float>::epsilon &&
				  abs<float>(    lIdentity.i[0][3]) < math::type_traits<float>::epsilon &&
				  abs<float>(    lIdentity.i[1][0]) < math::type_traits<float>::epsilon &&
				  abs<float>(    lIdentity.i[1][2]) < math::type_traits<float>::epsilon &&
				  abs<float>(    lIdentity.i[1][3]) < math::type_traits<float>::epsilon &&
				  abs<float>(    lIdentity.i[2][0]) < math::type_traits<float>::epsilon &&
				  abs<float>(    lIdentity.i[2][1]) < math::type_traits<float>::epsilon &&
				  abs<float>(    lIdentity.i[2][3]) < math::type_traits<float>::epsilon ;
		pragma_error_if(!lRetVal, "UnitTest: Inverse of matrix4x4 is not working properly.");


		return lRetVal;
	}
	
	bool DoMathTests()
	{
		return Vector2fTests() && Vector3fTests() && Vector4fTests() && Matrix3fTests() && Matrix4fTests();
	}

}
