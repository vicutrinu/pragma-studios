//
//  MyOpenGLView.m
//  pragma
//
//  Created by Victor on 30/01/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "MyOpenGLView.h"
#import <OpenGL/OpenGL.h>
#import <OpenGL/glu.h>

static float vertices[] = 
    {-1,  1,  1
    , 1,  1,  1
    ,-1, -1,  1
    , 1, -1,  1
    ,-1,  1, -1
    , 1,  1, -1
    ,-1, -1, -1
    , 1, -1, -1  };
static int indice[] = 
    { 0, 2, 1
    , 2, 3, 1
    , 1, 3, 5
    , 3, 7, 5
    , 5, 7, 4
    , 7, 6, 4
    , 4, 6, 0
    , 6, 2, 0
    , 4, 0, 5
    , 0, 1, 5
    , 2, 6, 3
    , 6, 7, 3 };

static float colors[] = 
    { 1, 0, 0
    , 0, 1, 0
    , 0, 0, 1
    , 1, 1, 0
    , 0, 1, 1
    , 1, 0, 1 };

// This is the renderer output callback function
static CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext)
{
    CVReturn result = [(MyOpenGLView*)displayLinkContext getFrameForTime:outputTime];
    return result;
}

@implementation MyOpenGLView

- (id)initWithCoder:(NSCoder*)coder
{
    if ((self = [super initWithCoder:coder]))
    {
        // Synchronize buffer swaps with vertical refresh rate
        GLint swapInt = 1;
        [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval]; 
        
        // Create a display link capable of being used with all active displays
        CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
        
        // Set the renderer output callback function
        CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, self);
        
        // Set the display link for the current renderer
        CGLContextObj cglContext = [[self openGLContext] CGLContextObj];
        CGLPixelFormatObj cglPixelFormat = [[self pixelFormat] CGLPixelFormatObj];
        CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
        
        // Activate the display link
        CVDisplayLinkStart(displayLink);
    }

    glEnable(GL_CULL_FACE);
    glMatrixMode( GL_PROJECTION );   // Select the projection matrix
    glLoadIdentity();                // and reset it
    // Calculate the aspect ratio of the view
    gluPerspective( 45.0f, 640 / 480, 0.1f, 100.0f );
    angle = 0;
    mPlay = false;    

    return self;
}

- (void)dealloc {
    // Clean-up code here.
    CVDisplayLinkRelease(displayLink);
    
    [super dealloc];
}

-(void) drawRect: (NSRect) bounds
{
    glClear(GL_COLOR_BUFFER_BIT);
    [self DrawScene];
    glFlush();
}

-(void) DrawScene
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef( 0, 0.0f, -6.0f );   // Left 1.5 units, into screen 6.0
    glRotatef(angle, 1, 1, 1);
    if(mPlay)
        angle+= 1;
    glBegin(GL_TRIANGLES);
    for(size_t i = 0; i < 36;++i)
    {
        float* lColor = &colors[(i/6)*3];
        glColor3f(lColor[0], lColor[1], lColor[2]);
        float* lVertex = &vertices[indice[i]*3];
        glVertex3f(lVertex[0], lVertex[1], lVertex[2]);
    }
    glEnd();
}

- (CVReturn)getFrameForTime:(const CVTimeStamp*)outputTime
{
    [self setNeedsDisplay:YES];
    // Add your drawing codes here
    
    return kCVReturnSuccess;
}

- (void) onPlay
{
    mPlay = true;
}

- (void) onPause
{
    mPlay = false;

}


@end
