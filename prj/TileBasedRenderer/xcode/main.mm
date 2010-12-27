//
//  main.m
//  Draw
//
//  Created by Victor on 2/7/10.
//  Copyright 2010 Apple Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include <pragma/data/data.h>

int main(int argc, char *argv[])
{
	pragma::PropertyList_UnitTest();
	
    return NSApplicationMain(argc,  (const char **) argv);
}
