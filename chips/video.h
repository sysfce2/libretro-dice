#ifndef VIDEO_H
#define VIDEO_H

class Video;

#include "../chip_desc.h"
#include "../video_desc.h"

class Video
{
private:
    uint64_t scanline_time;
    uint64_t current_time;
    uint64_t initial_time;

    uint32_t v_size;
    uint32_t v_pos;

    std::vector<double> color;

    void adjust_screen_params();
    void draw(Chip* chip);

public:
    VideoDesc* desc;
    uint32_t frame_count;
    enum VideoPins { HBLANK_PIN = 9, VBLANK_PIN = 10 };
    
    Video();
    void video_init(int width, int height, Circuit* circuit);
    static CUSTOM_LOGIC( video );
};

extern CHIP_DESC( VIDEO );

#endif
