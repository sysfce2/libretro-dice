#ifndef AUDIO_H
#define AUDIO_H

#include "../chip_desc.h"

class Audio
{
public:
    Audio();
    ~Audio();
    void audio_init(Circuit* circuit);
    static CUSTOM_LOGIC( audio );
    static void callback(void* userdata, uint8_t* str, int len);
    
private:
    std::vector<int16_t> audio_buffer[2];
    int current_buffer;
    int current_pos;
    uint64_t last_sample_time;
    uint64_t sample_period;
};

extern CHIP_DESC( AUDIO );

#endif
