#ifndef VIDEODESC_H
#define VIDEODESC_H

enum VideoOrientation { ROTATE_0 = 0, ROTATE_90, ROTATE_180, ROTATE_270 }; // Degrees clock-wise to rotate monitor

struct VideoDesc
{
    double r[9];
    double contrast;
    VideoOrientation orientation;

    VideoDesc() : contrast(1.0), orientation(ROTATE_0)
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
};

#define VIDEO_DESC( name ) VideoDesc video_##name = VideoDesc()
#define VIDEO_RESISTANCE( p, r ) .set_r(p, r)
#define VIDEO_CONTRAST( c ) .set_contrast(c)
#define VIDEO_ORIENTATION( o ) .set_orientation(o)
#define VIDEO_DESC_END ;

#endif
