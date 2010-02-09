//
//  MainView.m
//  TileBasedRenderer
//
//  Created by Victor on 2/8/10.
//  Copyright 2010 Apple Inc. All rights reserved.
//

#ifdef __OBJC__
#import <Cocoa/Cocoa.h>
#endif

#import "MainView.h"
#include "Raster.h"
#include <pragma/math/math.h> 
#include <pragma/math/functions.h>

@implementation MainView

static void rgbReleaseRGBDataProvider(void *info, const void *data, size_t size) {
    NSLog(@"call made to rgbReleaseRGBRampDataProvider");
    // free((char *)data);
} // end rgbReleaseRGBDataProvider

- (id)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
		windowRect = frame;
		mOk = false;
		
		cgColourSpaceRef        = [self obtainTheRGBColorSpace];
		
		mFrameBuffer = [self createBitmapWithRect:windowRect];    // strong? No.
		//NSInteger zIntMemorySizeInBytes = windowRect.size.width * windowRect.size.height * 3;
		
		mCgDataProvider    = [self createCGDataProviderWithBitmap:mFrameBuffer forRect:windowRect ];
		
		mOk = true;
		pragma::Raster::SetRenderContext(mFrameBuffer, 512, 512);
		
		mVertices[0] = pragma::Point(256, 100,10);
		mVertices[1] = pragma::Point(100, 350,10);
		mVertices[2] = pragma::Point(512-100, 350,10);
		mTimer = [NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)((1.0 / 30.0) * 1) target:self selector:@selector(drawView:) userInfo:nil repeats:TRUE];    }
    return self;
}

- (void)drawView:(id)sender
{
	[self setNeedsDisplay:true];

	pragma::matrix4x4<pragma::Real> lRot;
	for(int i = 0; i < 3; ++i)
	{
		pragma::Point lP;
		lP.x = ((mVertices[i].x-256) * pragma::Cos<pragma::Real>(0.01) + (mVertices[i].y-256) * pragma::Sin<pragma::Real>(0.01))+256;
		lP.y = ((mVertices[i].x-256) * -pragma::Sin<pragma::Real>(0.01) + (mVertices[i].y-256) * pragma::Cos<pragma::Real>(0.01))+256;
		mVertices[i] = lP;
	}
}

- (void)awakeFromNib {
    //cgColourSpaceRef        = [self obtainTheRGBColorSpace];
	
} // end awakeFromNib

- (unsigned char *)createBitmapWithRect:(NSRect)pRect {
    
    // Create a kCGImageAlphaNone RGB bitmap: Quartz p 216
    // format: 3 component, 24 bit data no alpha, no padding bytes
    // RGBRGBRGB... 
    // 8 bits per component, bits per pixel = 24, three component colour space RGB
    
    // allocate memory for 24bit rgb image    
    NSInteger    zIntMemorySizeInBytes    = pRect.size.width * pRect.size.height * 3;  // 3 = rgb
    zIntMemorySizeInBytes   *= sizeof(unsigned char);
    
    unsigned char * zUcharPtrToBitmapData = 
	(unsigned char *)NSAllocateCollectable( zIntMemorySizeInBytes, NSScannedOption);
    if(zUcharPtrToBitmapData == NULL) {
        NSLog(@"ERROR *** createBitmapWithRect: zUcharPtrToBitmapData == NULL");
        exit(1);
    } // end if
    
    return zUcharPtrToBitmapData;
    
} // end createBitmapWithRect

- (CGDataProviderRef) createCGDataProviderWithBitmap:(unsigned char *)pBitmap forRect:(NSRect)pRect {
	
    size_t zImagedataSize    = pRect.size.width * pRect.size.height * 3;
    CGDataProviderRef zCGDataProviderRef    = CGDataProviderCreateWithData(
																		   NULL,
																		   pBitmap,
																		   zImagedataSize,
																		   rgbReleaseRGBDataProvider);
	
    return zCGDataProviderRef;
    
} // end createCGDataProviderRefQuartzP189

- (CGImageRef) createCGImageUsingDataProvider:	(CGDataProviderRef)pDataProvider
												andColourSpace:(CGColorSpaceRef)pCgColorSpaceRefDisplay
                                                forRect:(NSRect)pRect {
    size_t    zBitsPerComponent        = 8;
    size_t    zBitsPerPixel            = 24;
    size_t    zBytesPerRow            = pRect.size.width * 3;
    bool    zBoolShouldInterpolate    = true;
    
    CGImageRef zCGImageRef    = CGImageCreate((size_t)pRect.size.width,
											  (size_t)pRect.size.height,
											  zBitsPerComponent,
											  zBitsPerPixel,
											  zBytesPerRow,
											  pCgColorSpaceRefDisplay,
											  kCGImageAlphaNone,
											  pDataProvider,
											  NULL,
											  zBoolShouldInterpolate,
											  kCGRenderingIntentDefault);

    if (zCGImageRef == NULL) {
        NSLog(@"MyDataProviderRGBBitmap *** ERROR *** createCGImageQuartzPage222UsingDataProvider");
        exit(1);
    }
    return zCGImageRef;               
    
} // end createCGImageQuartzPage222UsingColourSpace

- (CGColorSpaceRef) obtainTheRGBColorSpace  { 
    // pp 159 Programming with Quartz, Celphman and Laden    
    CMProfileRef    zGenericRGBProfile    = NULL;
    OSStatus        zOSStatusError        = noErr;
    CMProfileLocation    zLoc;
    
    // build up a profile location for ColorSync
    zLoc.locType        = cmPathBasedProfile;
    //strcpy(zLoc.u.pathLoc.path, kGenericRGBProfilePathStr);  // did not spend time making it work
    strcpy(zLoc.u.pathLoc.path, "/System/Library/ColorSync/Profiles/Generic RGB Profile.icc");
    
    // Open the profile with ColourSync
    zOSStatusError        = CMOpenProfile(&zGenericRGBProfile, &zLoc);
    
    if( !(zOSStatusError == noErr) ) {
        NSLog(@"MyPictureView *** ERROR *** getTheDisplayColorSpace CMOpenProfile");
        exit(1);
    } // 
    
    CGColorSpaceRef zCgColorSpaceRefDisplay = CGColorSpaceCreateWithPlatformColorSpace(zGenericRGBProfile);
    
    if(zCgColorSpaceRefDisplay == NULL) {
        NSLog(@"MyPictureView *** ERROR *** getTheDisplayColorSpace zCgColorSpaceRefDisplay");
        exit(1);
    } // end if
    
    return zCgColorSpaceRefDisplay;
    
} // end obtainTheRGBColorSpace

- (void)drawRect:(NSRect)dirtyRect {
    if(mOk)
	{
		pragma::Raster::ClearBackBuffer();
		pragma::Raster::RasterTriangle ( pragma::Raster::_Point2(mVertices[0].x, mVertices[0].y)
									   , pragma::Raster::_Point2(mVertices[1].x, mVertices[1].y)
									   , pragma::Raster::_Point2(mVertices[2].x, mVertices[2].y) );
		
		mImageRef = [self createCGImageUsingDataProvider: mCgDataProvider andColourSpace:cgColourSpaceRef forRect:windowRect];
		
		__strong NSGraphicsContext* nsGraphicsContext	= [NSGraphicsContext currentContext];
		__strong CGContextRef       zCgContextRef		= (CGContextRef) [nsGraphicsContext graphicsPort];
		
		CGRect    zCgRect    = NSRectToCGRect(dirtyRect);
		CGContextDrawImage(zCgContextRef,zCgRect,mImageRef);
		
		CGImageRelease(mImageRef);

	}
}

@end
