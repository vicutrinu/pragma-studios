/*
 *  TileRaster.h
 *  TileBasedRenderer
 *
 *  Created by Victor on 2/28/10.
 *  Copyright 2010 Apple Inc. All rights reserved.
 *
 */

#include <pragma/types.h>
#include <pragma/math/types.h>

namespace pragma { namespace TileRaster
{
	
	void SetRenderContext	( unsigned char* aBuffer, int aWidth, int aHeight );
	void ClearBackBuffer	();
	void Render				();
	void AddVertex			( const vector2f& aPosition );
	void AddColor			( const vector3f& aColor );
	
} }
