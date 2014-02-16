#ifndef VIDEO_WGL_H
#define VIDEO_WGL_H

#include <GL/gl.h>
#define glGetProcAddress(name) wglGetProcAddress(name)

#include "video.h"

class VideoWgl : public Video
{
private:
    HDC display;
    HGLRC wglcontext;
    HWND window;
    HINSTANCE glwindow;
    HWND handle;

public:
    VideoWgl(uintptr_t h) : Video(), handle((HWND)h) { }

    void video_init(int width, int height, Circuit* circuit)
    {
        if(wglcontext)
        {
            wglDeleteContext(wglcontext);
            wglcontext = 0;
        }

        GLuint pixel_format;
        PIXELFORMATDESCRIPTOR pfd;
        memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
        pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion   = 1;
        pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
    
        display = GetDC(handle);
        pixel_format = ChoosePixelFormat(display, &pfd);
        SetPixelFormat(display, pixel_format, &pfd);
    
        wglcontext = wglCreateContext(display);
        wglMakeCurrent(display, wglcontext);
    
        Video::video_init(width, height, circuit);
    }
    
    void swap_buffers()
    {
        SwapBuffers(display);
    }

    void show_cursor(bool show)
    {
        ShowCursor(show);
    }
};

#endif
