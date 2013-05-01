#ifndef AUDIODESC_H
#define AUDIODESC_H

struct AudioDesc
{
    double r[8];
    double gain;

    AudioDesc() : gain(1.0)
    {
        for(int i = 0; i < 8; i++) r[i] = 0.0;
    }
    AudioDesc& set_r(unsigned pin, double val) 
    { 
        if(pin > 0 && pin <= 8) r[pin-1] = val;
        return *this;
    }
    AudioDesc& set_gain(double g) 
    { 
        gain = g;
        return *this;
    }
};

#define AUDIO_DESC( name ) AudioDesc audio_##name = AudioDesc()
#define AUDIO_RESISTANCE( p, r ) .set_r(p, r)
#define AUDIO_GAIN( g ) .set_gain(g)
#define AUDIO_DESC_END ;

#endif
