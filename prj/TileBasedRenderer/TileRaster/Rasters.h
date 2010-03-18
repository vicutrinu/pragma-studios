#pragma once

namespace pragma { namespace TileRaster
{
	namespace Raster
	{
		/**
		 *	Definiciones de Rasters
		 *	Cada nuevo Raster debe heredar de la clase 'Base'. Por cada Iterador que utilice deberá
		 *	declarar los componentes y gradientes
		 *	Con esto indicaremos a los templates que deben incluir el codigo para iterar los valores
		 *	de color de vertice
		 */
		
		// Pinta un color solido configurado en el motor. No necesita iterar nada
		RASTER(Simple)
		{
			COMPONENTS	{ };
			GRADIENTS	{ };
		};
		
		// Pinta iterando los vertex colors
		RASTER(VertexColor)
		{
			COLOR_PRESENT;
			COMPONENTS	{ COLOR_COMPONENT };
			GRADIENTS	{ COLOR_GRADIENTS };
		};
		
		// Solo pinta la textura
		RASTER(Texture)
		{
			UV_PRESENT;
			COMPONENTS	{ UV_COMPONENT };
			GRADIENTS	{ UV_GRADIENTS };
		};
		
		// Multiplica el ColorVertex por la textura
		RASTER(TextureModulate)
		{
			COLOR_PRESENT;
			UV_PRESENT;
			COMPONENTS	{ COLOR_COMPONENT UV_COMPONENT };
			GRADIENTS	{ COLOR_GRADIENTS UV_GRADIENTS };
		};
		
	}
} }

