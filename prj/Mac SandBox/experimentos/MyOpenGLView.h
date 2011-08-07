//
//  MyClass.h
//  pragma
//
//  Created by Victor on 30/01/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <CoreVideo/CoreVideo.h>

@interface MyOpenGLView : NSOpenGLView {
@private
    CVDisplayLinkRef displayLink;
    float   angle;
    bool    mPlay;
}
- (void) drawRect: (NSRect) bounds;
- (void) DrawScene;
- (CVReturn)getFrameForTime:(const CVTimeStamp*)outputTime;
- (void) onPlay;
- (void) onPause;
@end
