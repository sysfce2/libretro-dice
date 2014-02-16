#ifndef VIDEO_CGL_H
#define VIDEO_CGL_H

#undef noinline
#define decimal CocoaDecimal
#import <Cocoa/Cocoa.h>
#include <OpenGL/gl.h>
#include <OpenGL/gl3.h>

#include "video.h"

class VideoCgl : public Video
{
private:
    NSOpenGLView* view;
    NSView* handle;

public:
    VideoCgl(uintptr_t h) : Video(), handle((NSView*)h), view(nil) { }

    void video_init(int width, int height, Circuit* circuit)
    {
        @autoreleasepool
        {
            [view removeFromSuperview];
            [view release];

            NSOpenGLPixelFormatAttribute attributes[] = {
                NSOpenGLPFAColorSize, 24,
                NSOpenGLPFAAlphaSize, 8,
                NSOpenGLPFADoubleBuffer,
                0
            };

            auto format = [[[NSOpenGLPixelFormat alloc] initWithAttributes:attributes] autorelease];
            auto context = [[[NSOpenGLContext alloc] initWithFormat:format shareContext:nil] autorelease];
            
            view = [[NSOpenGLView alloc] initWithFrame:NSMakeRect(0, 0, width, height) pixelFormat:format];
            [view setOpenGLContext:context];
            [handle addSubview:view];
            [context setView:view];
            
            [view lockFocus];

            Video::video_init(width, height, circuit);

            [view unlockFocus];
        }
    }
    
    void swap_buffers()
    {
         @autoreleasepool
         {
            if([view lockFocusIfCanDraw]) 
            {
                [[view openGLContext] flushBuffer];
                [view unlockFocus];
            }
        }
    }

    void show_cursor(bool show)
    {
        // TODO: Implement
    }
};

#endif
