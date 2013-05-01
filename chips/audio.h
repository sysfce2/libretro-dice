#ifndef AUDIO_H
#define AUDIO_H

#include "../chip_desc.h"
#include "../audio_desc.h"
#include "../cirque.h"

class Audio
{
public:
    AudioDesc* desc;
    
    Audio();
    ~Audio();
    void audio_init(Circuit* circuit);
    static CUSTOM_LOGIC( audio );
    static void callback(void* userdata, uint8_t* str, int len);
    

private:
    std::vector<double> gain;
    cirque<int16_t, 4096> audio_buffer;
    uint64_t last_sample_time;
    uint64_t sample_period;
};

extern CHIP_DESC( AUDIO );

#endif
