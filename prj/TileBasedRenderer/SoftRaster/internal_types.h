#pragma once

namespace pragma { namespace Raster
{

	struct InterpolatorsBase
	{
		// Interpoladores de posicion, comunes para todos los sabores de raster
		_Vector2 mLongEdge;
		_Vector2 mTopEdge;
		_Vector2 mBottomEdge;
		_Vector2 mSplit;
	};

	// Posible Vertex Formats
	namespace VertexFormat
	{

		// Interpolation params
		struct Position
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
		
		struct Color
		{
			struct Interpolators : public InterpolatorsBase
			{
				// Interpoladores de color
				_Color mLongColorEdge;
				_Color mTopColorEdge;
				_Color mBottomColorEdge;
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
		
		struct Normal
		{
			struct Interpolators : public InterpolatorsBase
			{
				// Interpoladores de color
				_Vector mLongNormalEdge;
				_Vector mTopNormalEdge;
				_Vector mBottomNormalEdge;
				_Vector mSplitNormal;
			};
			
			struct ScanlineParameters
			{
				struct Edge
				{
					_Vector mNormal;
					_Vector mNormalGradient;
				};
				Edge mLeft;
				Edge mRight;
				struct Increments
				{
					_Vector mNormalGradient;
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
		
		struct Color_Texture
		{
			struct Interpolators : public InterpolatorsBase
			{
				_Color mLongColorEdge;
				_Color mTopColorEdge;
				_Color mBottomColorEdge;
				_Color mSplitColor;
				UV mLongUVEdge;
				UV mTopUVEdge;
				UV mBottomUVEdge;
				UV mSplitUV;
			};
			
			struct ScanlineParameters
			{
				struct Edge
				{
					_Color mColor;
					_Color mColorGradient;
					UV mUV;
					UV mUVGradient;
				};
				Edge mLeft;
				Edge mRight;
				struct Increments
				{
					_Color mColorGradient;
					UV mUVGradient;
				} mIncrements;
			};
		};
		
	}

	// Raster function
	template<typename T, typename RASTERTYPE>
	inline void RasterLines	( Real& aLeft, Real& aRight, unsigned& aY
							, Real aLeftIncrement, Real aRightIncrement, unsigned aCount
							, typename T::ScanlineParameters::Increments& aIncrements
							, typename T::ScanlineParameters::Edge& aLeftParameters
							, typename T::ScanlineParameters::Edge& aRightParameters ) { /* Do Nothing */  }
	
	template<typename T>
	inline void InitColors(int i0, int i1, int i2, Real aVal, typename T::Interpolators& aTable)
	{
		aTable.mLongColorEdge				= sColors[i2] - sColors[i0];
		aTable.mTopColorEdge				= sColors[i1] - sColors[i0];
		aTable.mBottomColorEdge				= sColors[i2] - sColors[i1];
		aTable.mSplitColor					= sColors[i0] + (aTable.mLongColorEdge * aVal);
	}
	
	template<typename T>
	inline void AdjustScanlineColors( int i0, int i1, int i2, Real aScale1, Real aScale2
									, typename T::Interpolators& aTable, typename T::ScanlineParameters& aParameters)
	{
		aParameters.mLeft.mColor			= sColors[i0] + (aTable.mTopColorEdge * aScale1);
		aParameters.mRight.mColor			= sColors[i0] + (aTable.mLongColorEdge * aScale2);
		aParameters.mLeft.mColorGradient	= aTable.mTopColorEdge / aTable.mTopEdge.y;
		aParameters.mRight.mColorGradient	= aTable.mLongColorEdge / aTable.mLongEdge.y;
		aParameters.mIncrements.mColorGradient = ( aTable.mSplitColor - sColors[i1] ) / (aTable.mSplit.x - sPositions[i1].x);
	}
	
	template<typename T>
	inline void AdjustScanlineColors( int i0, int i1, int i2, Real aScale
									, typename T::Interpolators& aTable, typename T::ScanlineParameters& aParameters)
	{
		aParameters.mLeft.mColor			= sColors[i1] + (aTable.mBottomColorEdge * aScale);
		aParameters.mLeft.mColorGradient	= aTable.mBottomColorEdge / aTable.mBottomEdge.y;
	}

	
	template<typename T>
	inline void InitUVs(int i0, int i1, int i2, Real aVal, typename T::Interpolators& aTable)
	{
		aTable.mLongUVEdge					= sUVs[i2] - sUVs[i0];
		aTable.mTopUVEdge					= sUVs[i1] - sUVs[i0];
		aTable.mBottomUVEdge				= sUVs[i2] - sUVs[i1];
		aTable.mSplitUV						= sUVs[i0] + (aTable.mLongUVEdge * aVal);
	}
	
	template<typename T>
	inline void AdjustScanlineUVs( int i0, int i1, int i2, Real aScale1, Real aScale2
								 , typename T::Interpolators& aTable, typename T::ScanlineParameters& aParameters)
	{
		aParameters.mLeft.mUV				= sUVs[i0] + (aTable.mTopUVEdge * aScale1);
		aParameters.mRight.mUV				= sUVs[i0] + (aTable.mLongUVEdge * aScale2);
		aParameters.mLeft.mUVGradient		= aTable.mTopUVEdge / aTable.mTopEdge.y;
		aParameters.mRight.mUVGradient		= aTable.mLongUVEdge / aTable.mLongEdge.y;
		aParameters.mIncrements.mUVGradient = ( aTable.mSplitUV - sUVs[i1] ) / (aTable.mSplit.x - sPositions[i1].x);
	}
	
	template<typename T>
	inline void AdjustScanlineUVs( int i0, int i1, int i2, Real aScale
								 , typename T::Interpolators& aTable, typename T::ScanlineParameters& aParameters)
	{
		aParameters.mLeft.mUV			= sUVs[i1] + (aTable.mBottomUVEdge * aScale);
		aParameters.mLeft.mUVGradient	= aTable.mBottomUVEdge / aTable.mBottomEdge.y;
	}

	template<typename T>
	inline void InitNormals(int i0, int i1, int i2, Real aVal, typename T::Interpolators& aTable)
	{
		aTable.mLongNormalEdge				= sNormals[i2] - sNormals[i0];
		aTable.mTopNormalEdge				= sNormals[i1] - sNormals[i0];
		aTable.mBottomNormalEdge			= sNormals[i2] - sNormals[i1];
		aTable.mSplitNormal					= sNormals[i0] + (aTable.mLongNormalEdge * aVal);
	}
	
	template<typename T>
	inline void AdjustScanlineNormals( int i0, int i1, int i2, Real aScale1, Real aScale2
									 , typename T::Interpolators& aTable, typename T::ScanlineParameters& aParameters)
	{
		aParameters.mLeft.mNormal			= sNormals[i0] + (aTable.mTopNormalEdge * aScale1);
		aParameters.mRight.mNormal			= sNormals[i0] + (aTable.mLongNormalEdge * aScale2);
		aParameters.mLeft.mNormalGradient	= aTable.mTopNormalEdge / aTable.mTopEdge.y;
		aParameters.mRight.mNormalGradient	= aTable.mLongNormalEdge / aTable.mLongEdge.y;
		aParameters.mIncrements.mNormalGradient = ( aTable.mSplitNormal - sNormals[i1] ) / (aTable.mSplit.x - sPositions[i1].x);
	}
	
	template<typename T>
	inline void AdjustScanlineNormals( int i0, int i1, int i2, Real aScale
									 , typename T::Interpolators& aTable, typename T::ScanlineParameters& aParameters)
	{
		aParameters.mLeft.mNormal			= sNormals[i1] + (aTable.mBottomNormalEdge * aScale);
		aParameters.mLeft.mNormalGradient	= aTable.mBottomNormalEdge / aTable.mBottomEdge.y;
	}
	
#define NULL_INTERPOLATOR(aName, aFormat) \
	template<>\
	inline void aName<VertexFormat::aFormat>(int i0, int i1, int i2, Real aVal, VertexFormat::aFormat::Interpolators& aTable) { /* Do Nothing */ }
	
#define NULL_ADJUST(aName, aFormat) \
	template<>\
	inline void aName<VertexFormat::aFormat>	( int i0, int i1, int i2, Real aScale1, Real aScale2\
												, VertexFormat::aFormat::Interpolators& aTable, VertexFormat::aFormat::ScanlineParameters& aParameters) { /* Do Nothing */ }\
	\
	template<>\
	inline void aName<VertexFormat::aFormat>	( int i0, int i1, int i2, Real aScale\
												, VertexFormat::aFormat::Interpolators& aTable, VertexFormat::aFormat::ScanlineParameters& aParameters) { /* Do Nothing */ }

#define RASTER(aRasterName, aVertexFormat) \
	struct aRasterName \
	{ \
		typedef VertexFormat::aVertexFormat InterpolatorType; \
	}; \

	// Dependiendo de los formatos de vertice creamos funciones de interpolacion vacias para aquellos componentes que no se encuentran
	//	presentes en el formato, y por lo tanto no hacen falta ser interpolados
	NULL_INTERPOLATOR(InitColors,		Position)
	NULL_INTERPOLATOR(InitUVs,			Position)
	NULL_INTERPOLATOR(InitNormals,      Position)
	NULL_INTERPOLATOR(InitColors,		Texture)
	NULL_INTERPOLATOR(InitNormals,		Texture)
	NULL_INTERPOLATOR(InitUVs,			Color)
	NULL_INTERPOLATOR(InitNormals,		Color)
	NULL_INTERPOLATOR(InitNormals,		Color_Texture)
	NULL_INTERPOLATOR(InitColors,		Normal)
	NULL_INTERPOLATOR(InitUVs,			Normal)
	// Lo mismo para las funciones que ajustan el subpixel
	NULL_ADJUST(AdjustScanlineColors,	Position)
	NULL_ADJUST(AdjustScanlineUVs,		Position)
	NULL_ADJUST(AdjustScanlineNormals,  Position)
	NULL_ADJUST(AdjustScanlineColors,	Texture)
	NULL_ADJUST(AdjustScanlineNormals,	Texture)
	NULL_ADJUST(AdjustScanlineUVs,		Color)
	NULL_ADJUST(AdjustScanlineNormals,	Color)
	NULL_ADJUST(AdjustScanlineNormals,	Color_Texture)
	NULL_ADJUST(AdjustScanlineColors,	Normal)
	NULL_ADJUST(AdjustScanlineUVs,		Normal)

	// Rasters	Raster Name					Vertex Format
	RASTER	(	SimpleRaster				, Position)
	RASTER	(	VertexColorRaster			, Color)
	RASTER	(	TextureRaster				, Texture)
	RASTER	(	TextureModulateRaster		, Color_Texture)
	RASTER	(	GouraudRaster				, Normal)
										 
} }
