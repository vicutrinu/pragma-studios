#pragma once

namespace pragma { namespace Raster
{

	struct InterpolatorsBase
	{
		// Interpoladores de posicion, comunes para todos los sabores de raster
		_Vector2 mLongEdge;
		_Vector2 mTopShortEdge;
		_Vector2 mBottomShortEdge;
		_Vector2 mSplit;
	};
	
	// Interpolation params
	struct Draft
	{
		struct Interpolators : public InterpolatorsBase
		{
		};
		struct ScanlineParameters
		{
			struct Edge
			{
			};
			Edge mLeft;
			Edge mRight;
			struct Increments
			{
			} mIncrements;
		};
	};
	
	struct ColorVertex
	{
		struct Interpolators : public InterpolatorsBase
		{
			// Interpoladores de color
			_Color mLongColorEdge;
			_Color mTopShortColorEdge;
			_Color mBottomShortColorEdge;
			_Color mSplitColor;
		};
		
		struct ScanlineParameters
		{
			struct Edge
			{
				_Color mColor;
				_Color mColorGradient;
			};
			Edge mLeft;
			Edge mRight;
			struct Increments
			{
				_Color mColorGradient;
			} mIncrements;
		};
		
	};
	
	struct Texture
	{
		struct Interpolators : public InterpolatorsBase
		{
			// Interpoladores de color
			UV mLongUVEdge;
			UV mTopUVEdge;
			UV mBottomUVEdge;
			UV mSplitUV;
		};
		
		struct ScanlineParameters
		{
			struct Edge
			{
				UV mUV;
				UV mUVGradient;
			};
			Edge mLeft;
			Edge mRight;
			struct Increments
			{
				UV mUVGradient;
			} mIncrements;
		};
	};
	
	struct ColorVertex_Normal
	{
		struct Interpolators 
		{
			// Interpoladores de color
			_Color mLongColorEdge;
			_Color mTopShortColorEdge;
			_Color mBottomShortColorEdge;
			_Color mSplitColor;
			_Vector mLongNormal;
			_Vector mTopNormalEdge;
			_Vector mBottomNormaleEdge;
			_Vector mSplitNormal;
		};
		
		struct ScanlineParameters
		{
			struct Edge
			{
				_Color mColor;
				_Color mColorGradient;
				_Vector mNormal;
				_Vector mNormalGradient;
			};
			Edge mLeft;
			Edge mRight;
			struct Increments
			{
				_Color mColorGradient;
				_Vector mNormalGradient;
			} mIncrements;
		};
	};

	// Raster function
	template<typename T>
	inline void RasterLines	( Real& aLeft, Real& aRight, unsigned& aY
							, Real aLeftIncrement, Real aRightIncrement, unsigned aCount
							, typename T::ScanlineParameters::Increments& aIncrements
							, typename T::ScanlineParameters::Edge& aLeftParameters
							, typename T::ScanlineParameters::Edge& aRightParameters ) { /* Do Nothing */  }

#define NULL_INTERPOLATOR(aName, aFormat) \
	template<>\
	inline void aName<aFormat>(int i0, int i1, int i2, Real aVal, aFormat::Interpolators& aTable) { /* Do Nothing */ }
	
#define NULL_ADJUST(aName, aFormat) \
	template<>\
	inline void aName<aFormat>	( int i0, int i1, int i2, Real aScale1, Real aScale2, aFormat::Interpolators& aTable, aFormat::ScanlineParameters& aParameters) { /* Do Nothing */ }\
	\
	template<>\
	inline void aName<aFormat>	( int i0, int i1, int i2, Real aScale\
								, aFormat::Interpolators& aTable, aFormat::ScanlineParameters& aParameters) { /* Do Nothing */ }
	
												 
} }
