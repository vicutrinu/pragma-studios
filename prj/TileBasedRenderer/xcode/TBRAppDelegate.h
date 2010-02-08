//
//  TBRAppDelegate.h
//  TileBasedRenderer
//
//  Created by Victor on 2/8/10.
//  Copyright 2010 Apple Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface TBRAppDelegate : NSObject <NSApplicationDelegate> 
{
	NSWindow *window;
}

@property (assign) IBOutlet NSWindow* window;

@end
