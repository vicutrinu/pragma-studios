#pragma once

namespace pragma { namespace TileRaster
{
	
	// Types
	typedef float			Real;
	typedef vector2<Real>	Position2;
	typedef vector3<Real>	Position;
	typedef vector3<Real>	Color;
	typedef vector3<Real>	UV;

#define TILE_SIZE 16
	
#define RASTER(aName)	struct aName : public Base 
#define COMPONENTS		struct Start
#define GRADIENTS		struct Gradients
#define COLOR_COMPONENT TileRaster::Color	mColor;
#define COLOR_GRADIENTS TileRaster::Color	mColorX; TileRaster::Color mColorY;
#define UV_COMPONENT	TileRaster::UV		mUV;
#define UV_GRADIENTS	TileRaster::UV		mUVX; TileRaster::UV mUVY;
#define COLOR_PRESENT	typedef Present Color
#define UV_PRESENT		typedef Present UV

	namespace Raster
	{		
		struct Present { };
		struct NotPresent { };
		
		struct Base
		{
			typedef NotPresent Color;
			typedef NotPresent UV;
		};
	}

	// Templates
	template<typename T>
	void RasterTile							( uint32* aData, const typename T::Start* aStartValue, const typename T::Gradients* aGradients ) { }
	
	template<typename T>
	void RasterTile							( const uint8* aMask, uint32* aData, const typename T::Start* aStartValue, const typename T::Gradients* aGradients ) { }
	
	template<typename COMPONENT_PRESENT>
	class Gradient
	{
	public:
		template<typename T>
		static inline void GetColor			( typename T::Gradients* aGradients, Real  aValX, Real aValY, int X0, int X1, int X2, int Y0, int Y1, int Y2, Real aLengthX, Real aLengthY) { }
		
		template<typename T>
		static inline void GetUV			( typename T::Gradients* aGradients, Real aValX, Real aValY, int X0, int X1, int X2, int Y0, int Y1, int Y2, Real aLengthX, Real aLengthY) { }
	};
	
	template<typename COMPONENT_PRESENT>
	class Start
	{
	public:
		template<typename T>
		static inline void GetColor			( int aVertex, typename T::Gradients& aGradients, typename T::Start& aStarts) { }
		
		template<typename T>
		static inline void GetUV			( int aVertex, typename T::Gradients& aGradients, typename T::Start& aStarts) { }
	};
	
	template<typename COMPONENT_PRESENT>
	class TileSetup
	{
	public:
		template<typename T>
		static inline void MoveColor_Right	( int aCount, typename T::Start& aPos, const typename T::Gradients& aGradients ) { }
		template<typename T>
		static inline void MoveColor_Down	( int aCount, typename T::Start& aPos, const typename T::Gradients& aGradients ) { }
		template<typename T>
		static inline void MoveUV_Right		( int aCount, typename T::Start& aPos, const typename T::Gradients& aGradients ) { }
		template<typename T>
		static inline void MoveUV_Down		( int aCount, typename T::Start& aPos, const typename T::Gradients& aGradients ) { }
	};
	
	template<>
	class TileSetup<Raster::Present>
	{
	public:
		template<typename T>
		static inline void MoveColor_Right( int aCount, typename T::Start& aPos, const typename T::Gradients& aGradients )
		{
			aPos.mColor = aPos.mColor + (aGradients.mColorX * Real(TILE_SIZE * aCount));
		}
		template<typename T>
		static inline void MoveColor_Down ( int aCount, typename T::Start& aPos, const typename T::Gradients& aGradients )
		{
			aPos.mColor = aPos.mColor + (aGradients.mColorY * Real(TILE_SIZE * aCount));
		}
		template<typename T>
		static inline void MoveUV_Right   ( int aCount, typename T::Start& aPos, const typename T::Gradients& aGradients )
		{
			//Not implemented yet!
		}
		template<typename T>
		static inline void MoveUV_Down    ( int aCount, typename T::Start& aPos, const typename T::Gradients& aGradients )
		{
			//Not implemented yet!
		}
	};
	
} }
