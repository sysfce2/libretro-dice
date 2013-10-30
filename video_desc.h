#ifndef VIDEODESC_H
#define VIDEODESC_H

#include <vector>

enum VideoOrientation { ROTATE_0 = 0, ROTATE_90, ROTATE_180, ROTATE_270 }; // Degrees clock-wise to rotate monitor
enum VideoScanMode { PROGRESSIVE = 0, INTERLACED };

struct VideoOverlay
{
    double x, y;
    double width, height; // NOTE: Negative values for width or height specify maximum possible value
    double r, g, b;
};

struct VideoDesc
{
    double r[9];
    double contrast;
    VideoOrientation orientation;
    VideoScanMode scan_mode;
    std::vector<VideoOverlay> overlays;

    VideoDesc() : contrast(1.0), orientation(ROTATE_0), scan_mode(PROGRESSIVE)
    {
        for(int i = 0; i < 9; i++) r[i] = 0.0;
    }
    VideoDesc& set_r(unsigned pin, double val) 
    { 
        if(pin > 0 && pin <= 9) r[pin-1] = val;
        return *this;
    }
    VideoDesc& set_contrast(double c) 
    { 
        contrast = c;
        return *this;
    }
    VideoDesc& set_orientation(VideoOrientation o) 
    { 
        orientation = o;
        return *this;
    }
    VideoDesc& set_scan_mode(VideoScanMode s)
    {
        scan_mode = s;
        return *this;
    }
    VideoDesc& add_overlay(const VideoOverlay& o)
    {
        overlays.push_back(o);
        return *this;
    }

    static const VideoDesc DEFAULT;
};

#define VIDEO_DESC( name ) VideoDesc video_##name = VideoDesc()
#define VIDEO_RESISTANCE( p, r ) .set_r(p, r)
#define VIDEO_CONTRAST( c ) .set_contrast(c)
#define VIDEO_ORIENTATION( o ) .set_orientation(o)
#define VIDEO_SCAN_MODE( s ) .set_scan_mode(s)
#define VIDEO_OVERLAY( o... ) .add_overlay({o})
#define VIDEO_DESC_END ;

#endif
