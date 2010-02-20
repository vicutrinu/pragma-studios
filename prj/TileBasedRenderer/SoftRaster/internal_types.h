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
	template<typename T, typename RASTERTYPE>
	inline void RasterLines	( Real& aLeft, Real& aRight, unsigned& aY
							, Real aLeftIncrement, Real aRightIncrement, unsigned aCount
							, typename T::ScanlineParameters::Increments& aIncrements
							, typename T::ScanlineParameters::Edge& aLeftParameters
							, typename T::ScanlineParameters::Edge& aRightParameters ) { /* Do Nothing */  }
	
	template<typename T>
	inline void InterpolateColors(int i0, int i1, int i2, Real aVal, typename T::Interpolators& aTable)
	{
		aTable.mLongColorEdge			= sColors[i2] - sColors[i0];
		aTable.mTopShortColorEdge		= sColors[i1] - sColors[i0];
		aTable.mBottomShortColorEdge	= sColors[i2] - sColors[i1];
		aTable.mSplitColor				= sColors[i0] + (aTable.mLongColorEdge * aVal);
	}
	
	template<typename T>
	inline void InterpolateUVs(int i0, int i1, int i2, Real aVal, typename T::Interpolators& aTable)
	{
		aTable.mLongUVEdge		= sUVs[i2] - sUVs[i0];
		aTable.mTopUVEdge		= sUVs[i1] - sUVs[i0];
		aTable.mBottomUVEdge	= sUVs[i2] - sUVs[i1];
		aTable.mSplitUV			= sUVs[i0] + (aTable.mLongUVEdge * aVal);
	}
	
	template<typename T>
	inline void AdjustScanlineColors( int i0, int i1, int i2, Real aScale1, Real aScale2
									 , typename T::Interpolators& aTable, typename T::ScanlineParameters& aParameters)
	{
		aParameters.mLeft.mColor			= sColors[i0] + (aTable.mTopShortColorEdge * aScale1);
		aParameters.mRight.mColor			= sColors[i0] + (aTable.mLongColorEdge * aScale2);
		aParameters.mLeft.mColorGradient	= aTable.mTopShortColorEdge / aTable.mTopShortEdge.y;
		aParameters.mRight.mColorGradient	= aTable.mLongColorEdge / aTable.mLongEdge.y;
		aParameters.mIncrements.mColorGradient = ( aTable.mSplitColor - sColors[i1] ) / (aTable.mSplit.x - sPositions[i1].x);
	}
	
	template<typename T>
	inline void AdjustScanlineColors( int i0, int i1, int i2, Real aScale
									 , typename T::Interpolators& aTable, typename T::ScanlineParameters& aParameters)
	{
		aParameters.mLeft.mColor			= sColors[i1] + (aTable.mBottomShortColorEdge * aScale);
		aParameters.mLeft.mColorGradient	= aTable.mBottomShortColorEdge / aTable.mBottomShortEdge.y;
	}
	
	template<typename T>
	inline void AdjustScanlineNormals( int i0, int i1, int i2, Real aScale1, Real aScale2
									  , typename T::Interpolators& aTable, typename T::ScanlineParameters& aParameters)
	{
		aParameters.mLeft.mNormal			= sNormals[i0] + (aTable.mTopShortColorEdge * aScale1);
		aParameters.mRight.mNormal			= sNormals[i0] + (aTable.mLongColorEdge * aScale2);
		aParameters.mLeft.mColorGradient	= aTable.mTopNormalEdge / aTable.mTopShortEdge.y;
		aParameters.mRight.mColorGradient	= aTable.mLongNormalEdge / aTable.mLongEdge.y;
		aParameters.mIncrements.mNormalGradient = ( aTable.mSplitNormal - sNormals[i1] ) / (aTable.mSplit.x - sPositions[i1].x);
	}
	
	template<typename T>
	inline void AdjustScanlineNormals( int i0, int i1, int i2, Real aScale
									  , typename T::Interpolators& aTable, typename T::ScanlineParameters& aParameters)
	{
		aParameters.mLeft.mNormal			= sColors[i1] + (aTable.mBottomNormalEdge * aScale);
		aParameters.mLeft.mNormalGradient	= aTable.mBottomNormalEdge / aTable.mBottomShortEdge.y;
	}
	
	template<typename T>
	inline void AdjustScanlineUVs( int i0, int i1, int i2, Real aScale1, Real aScale2
								  , typename T::Interpolators& aTable, typename T::ScanlineParameters& aParameters)
	{
		aParameters.mLeft.mUV				= sUVs[i0] + (aTable.mTopUVEdge * aScale1);
		aParameters.mRight.mUV				= sUVs[i0] + (aTable.mLongUVEdge * aScale2);
		aParameters.mLeft.mUVGradient		= aTable.mTopUVEdge / aTable.mTopShortEdge.y;
		aParameters.mRight.mUVGradient		= aTable.mLongUVEdge / aTable.mLongEdge.y;
		aParameters.mIncrements.mUVGradient = ( aTable.mSplitUV - sUVs[i1] ) / (aTable.mSplit.x - sPositions[i1].x);
	}
	
	template<typename T>
	inline void AdjustScanlineUVs( int i0, int i1, int i2, Real aScale
								  , typename T::Interpolators& aTable, typename T::ScanlineParameters& aParameters)
	{
		aParameters.mLeft.mUV			= sUVs[i1] + (aTable.mBottomUVEdge * aScale);
		aParameters.mLeft.mUVGradient	= aTable.mBottomUVEdge / aTable.mBottomShortEdge.y;
	}

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

	struct DraftRaster
	{
		typedef Draft InterpolatorType;
	};
	
	struct VertexColorRaster
	{
		typedef ColorVertex InterpolatorType;
	};
	
	struct TextureRaster
	{
		typedef Texture InterpolatorType;
	};
												 
} }
