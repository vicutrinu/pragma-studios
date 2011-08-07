//
//  pragmaAppDelegate.h
//  pragma
//
//  Created by Victor on 30/01/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "MyOpenGLView.h"

@interface pragmaAppDelegate : NSObject <NSApplicationDelegate> {
    NSWindow *window;
    NSWindow *window2;
    NSButton *button;
    NSButton *button2;
    MyOpenGLView *glView;
    
}

@property (assign) IBOutlet NSWindow *window;
@property (assign) IBOutlet NSWindow *window2;
@property (assign) IBOutlet NSButton *button;
@property (assign) IBOutlet NSButton *button2;

@property (assign) IBOutlet MyOpenGLView *glView;

-(IBAction) pollas: (id) sender;
-(IBAction) pollas2: (id) sender;

@end
