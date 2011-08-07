//
//  pragmaAppDelegate.m
//  pragma
//
//  Created by Victor on 30/01/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "experimentosAppDelegate.h"

@implementation pragmaAppDelegate

@synthesize window;
@synthesize window2;
@synthesize button;
@synthesize button2;
@synthesize glView;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    [window2 setTitle:@"como me mola trastear"];
    [window setReleasedWhenClosed:FALSE];
}

-(IBAction) pollas: (id) sender {
    NSLog(@"User clicked %@", sender);
    NSButton* lButton = (NSButton*)sender;
    if([lButton state] == NSOnState)
    {
        [button2 setState:NSOffState];
        [glView onPlay];
    }
    else
    {
        [button2 setState:NSOnState];
        [glView onPause];
    }
    // Do something here with the variable 'sender'
    if([window isVisible] == NO)
        [window makeKeyAndOrderFront:self];
}

-(IBAction) pollas2: (id) sender {
    NSLog(@"User clicked %@", sender);
    NSButton* lButton = (NSButton*)sender;
    if([lButton state] == NSOnState)
    {
        [glView onPause];
        [button setState:NSOffState];
    }
    else
    {
        [glView onPlay];
        [button setState:NSOnState];
    }
    // Do something here with the variable 'sender'
    if([window isVisible] == NO)
        [window makeKeyAndOrderFront:self];
}

@end
