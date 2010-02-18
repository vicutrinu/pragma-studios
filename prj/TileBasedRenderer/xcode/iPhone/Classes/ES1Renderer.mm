//
//  ES1Renderer.m
//  Untitled
//
//  Created by Victor on 2/16/10.
//  Copyright Apple Inc 2010. All rights reserved.
//

#import "ES1Renderer.h"

#include "Raster.h"
#include <pragma/math/math.h> 
#include <pragma/math/functions.h>

@implementation ES1Renderer

// Create an OpenGL ES 1.1 context
- (id)init
{
    if (self = [super init])
    {
        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];

        if (!context || ![EAGLContext setCurrentContext:context])
        {
            [self release];
            return nil;
        }

        // Create default framebuffer object. The backing will be allocated for the current layer in -resizeFromLayer
        glGenFramebuffersOES(1, &defaultFramebuffer);
        glGenRenderbuffersOES(1, &colorRenderbuffer);
        glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
        glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, colorRenderbuffer);

		mVertices[0] = pragma::Point((160-100), (240-100),10);
		mVertices[1] = pragma::Point((160+100), (240-100),10);
		mVertices[2] = pragma::Point((160-100), (240+100),10);
		mVertices[3] = pragma::Point((160+100), (240+100),10);
		
		/*mVertices[0] = pragma::Point(0, 0,10);
		mVertices[1] = pragma::Point(320, 0,10);
		mVertices[2] = pragma::Point(0, 480,10);
		mVertices[3] = pragma::Point(320, 480,10);*/
		
		mBuffer = (unsigned char*)malloc(512*512*4);
		memset(mBuffer, 32, 512*512*4);
		
		glGenTextures(1, &mTexture);
		GLenum lError = glGetError();
		glBindTexture(GL_TEXTURE_2D, mTexture);
		lError = glGetError();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, mBuffer);
		lError = glGetError();
		glEnable(GL_TEXTURE_2D);
		lError = glGetError();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
    }

    return self;
}

- (void)render
{
    // Replace the implementation of this method to do your own custom drawing

    static const GLfloat squareVertices[] = {
        -1.f,  1.f,
         1.f,  1.f,
        -1.f, -1.f,
         1.f, -1.f,
    };

	static const GLfloat squareUVs[] = {
		0, 0,
		320.f/512, 0,
		0, 480.f/512, 
		320.f/512, 480.f/512
	};

    // This application only creates a single context which is already set current at this point.
    // This call is redundant, but needed if dealing with multiple contexts.
    [EAGLContext setCurrentContext:context];

    // This application only creates a single default framebuffer which is already bound at this point.
    // This call is redundant, but needed if dealing with multiple framebuffers.
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mTexture);
    glViewport(0, 0, backingWidth, backingHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glVertexPointer(2, GL_FLOAT, 0, squareVertices);
    glEnableClientState(GL_VERTEX_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, squareUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // This application only creates a single color renderbuffer which is already bound at this point.
    // This call is redundant, but needed if dealing with multiple renderbuffers.
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
    [context presentRenderbuffer:GL_RENDERBUFFER_OES];
	
	pragma::matrix4x4<pragma::Real> lRot;
	for(int i = 0; i < 4; ++i)
	{
		pragma::Point lP;
		lP.x = ((mVertices[i].x-160) * pragma::Cos<pragma::Real>(0.01) + (mVertices[i].y-240) * pragma::Sin<pragma::Real>(0.01))+160;
		lP.y = ((mVertices[i].x-160) * -pragma::Sin<pragma::Real>(0.01) + (mVertices[i].y-240) * pragma::Cos<pragma::Real>(0.01))+240;
		mVertices[i] = lP;
	}
	pragma::Raster::SetRenderContext(mBuffer, 512, 512);
	pragma::Raster::ClearBackBuffer();
	pragma::Raster::ClearBackBuffer();
	pragma::Raster::VertexColor( pragma::Raster::_Color(1,0,0) );
	pragma::Raster::VertexUV( pragma::Raster::UV(0,0) );
	pragma::Raster::AddVertex( pragma::Raster::_Point2(mVertices[0].x, mVertices[0].y) );
	pragma::Raster::VertexColor( pragma::Raster::_Color(0,1,0) );
	pragma::Raster::VertexUV( pragma::Raster::UV(0,1) );
	pragma::Raster::AddVertex( pragma::Raster::_Point2(mVertices[1].x, mVertices[1].y) );
	pragma::Raster::VertexColor( pragma::Raster::_Color(0,0,1) );
	pragma::Raster::VertexUV( pragma::Raster::UV(1,0) );
	pragma::Raster::AddVertex( pragma::Raster::_Point2(mVertices[2].x, mVertices[2].y) );
	
	pragma::Raster::VertexColor( pragma::Raster::_Color(0,1,0) );
	pragma::Raster::VertexUV( pragma::Raster::UV(0,1) );
	pragma::Raster::AddVertex( pragma::Raster::_Point2(mVertices[1].x, mVertices[1].y) );
	pragma::Raster::VertexColor( pragma::Raster::_Color(0,0,1) );
	pragma::Raster::VertexUV( pragma::Raster::UV(1,0) );
	pragma::Raster::AddVertex( pragma::Raster::_Point2(mVertices[2].x, mVertices[2].y) );
	pragma::Raster::VertexColor( pragma::Raster::_Color(1,1,1) );
	pragma::Raster::VertexUV( pragma::Raster::UV(1,1) );
	pragma::Raster::AddVertex( pragma::Raster::_Point2(mVertices[3].x, mVertices[3].y) );
	pragma::Raster::Render();
	pragma::Raster::Render();
	glBindTexture(GL_TEXTURE_2D, mTexture);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, mBuffer);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 512, 480, GL_RGBA, GL_UNSIGNED_BYTE, mBuffer);
}

- (BOOL)resizeFromLayer:(CAEAGLLayer *)layer
{	
    // Allocate color buffer backing based on the current layer size
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
    [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:layer];
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);

    if (glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES)
    {
        NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
        return NO;
    }

    return YES;
}

- (void)dealloc
{
    // Tear down GL
    if (defaultFramebuffer)
    {
        glDeleteFramebuffersOES(1, &defaultFramebuffer);
        defaultFramebuffer = 0;
    }

    if (colorRenderbuffer)
    {
        glDeleteRenderbuffersOES(1, &colorRenderbuffer);
        colorRenderbuffer = 0;
    }

    // Tear down context
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];

    [context release];
    context = nil;

    [super dealloc];
}

@end
