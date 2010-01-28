#include <stdio.h>

#include <pragma/graphics/types.h>
#include <pragma/graphics/Mesh.h>

namespace pragma
{
	struct TVertex
	{
		Point	mPoint;
		Vector2	mUV;
		uint32  mColor;
	};
	
#define TILE_SIZE 32
	
	class Drawer
	{
		// Lo que viene siendo un Pixel Shader / Combiner, con su Sampler de texturas y toda
		//	la pesca
	};
	
	class Tile
	{
	public:
		void AddTriangle(uint16 aTriangleIndex);
		
		void HiddenSurfaceRemoval()
		{
			// Eliminamos triangulos completamente ocultos
		}
		
		void Render(Drawer& aDrawer)
		{
			HiddenSurfaceRemoval(); // ocultar por completo los triangulos que quedan por detras de otros
			// Por cada pixel determinar a que triangulo pertenece, calcular los valores interpolados
			//	y pasarle los resultados al objeto aDrawer
		}
	private:
		std::vector<uint16> mTriangles;		
	};
	
	class TileBasedRenderer
	{
	public:
		TileBasedRenderer( size_t aWidth, size_t aHeight )
						  : mWidth(aWidth)
						  , mHeight(aHeight)
		{
			mHorizontalTiles = mWidth / TILE_SIZE;
			mHorizontalTiles+= (mWidth%TILE_SIZE) == 0? 0 : 1;
			mVerticalTiles = mHeight / TILE_SIZE;
			mVerticalTiles+= (mHeight%TILE_SIZE) == 0? 0 : 1;
			
			mTiles = new Tile[mHorizontalTiles * mVerticalTiles];
		}
		
		~TileBasedRenderer()
		{
			delete[] mTiles;
		}
		
		void DrawTriangles(TVertex* aVertices, uint16* aIndices)
		{
			// Por cada triangulo, insertarlo en los tiles correspondientes
		}
		
		void Render()
		{
			Drawer lDrawer;
			
			// Los tiles se pueden procesar en paralelo, usando tantos drawers como tiles se ejecuten al mismo tiempo
			// La composicion de la imagen tb ha de ser concurrente
			// El procesado de un Tile lo puede llevar a cabo un Thread, o un Kernel de OpenCL, o incluso una PPE de un Cell
			for( size_t j = 0; j < mVerticalTiles; ++j)
			{
				for(size_t i = 0; i < mHorizontalTiles; ++i)
				{
					mTiles[ j*mHorizontalTiles + i ].Render(lDrawer);
					ComposeImage(lDrawer, i*TILE_SIZE, j*TILE_SIZE);
				}
			}
		}
		
	private:
		void ComposeImage(const Drawer& aDrawer, size_t aPosX, size_t aPosY)
		{
		}
		
		size_t mWidth;
		size_t mHeight;
		
		size_t mHorizontalTiles;
		size_t mVerticalTiles;
		
		Tile* mTiles;
	};
}

using namespace pragma;

int main(int argc, char* argv[])
{
	TileBasedRenderer lRenderer(320, 240);
	
	Mesh lMesh("../../SimpleObject.mesh");

	TVertex* lVertices;
	uint16* lIndices;
	
	lRenderer.DrawTriangles(lVertices, lIndices);
	lRenderer.Render();
	
	return 0;
}