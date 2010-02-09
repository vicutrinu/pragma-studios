//
//  MainView.h
//  TileBasedRenderer
//
//  Created by Victor on 2/8/10.
//  Copyright 2010 Apple Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include <pragma/graphics/types.h>


@interface MainView : NSView {
	NSRect			windowRect;
	CGColorSpaceRef	cgColourSpaceRef;
	bool			mOk;
	
	unsigned char*	mFrameBuffer;
	CGImageRef		mImageRef;
	CGDataProviderRef mCgDataProvider;
	NSTimer*		mTimer;
	
	pragma::Point   mVertices[4];
}

- (CGColorSpaceRef) obtainTheRGBColorSpace;
- (unsigned char *)createBitmapWithRect:(NSRect)pRect;
- (CGDataProviderRef) createCGDataProviderWithBitmap:(unsigned char *)pBitmap forRect:(NSRect)pRect;
- (CGImageRef) createCGImageUsingDataProvider:	(CGDataProviderRef)pDataProvider 
												andColourSpace: (CGColorSpaceRef)pCgColorSpaceRefDisplay
												forRect: (NSRect)pRect;
- (void)drawView:(id)sender;

@end
