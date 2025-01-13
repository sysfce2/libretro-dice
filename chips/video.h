#ifndef VIDEO_H
#define VIDEO_H

class Video;

#include "../chip_desc.h"
#include "../video_desc.h"
#include "../settings.h"

class Video
{
protected:
    uint64_t scanline_time;
    uint64_t current_time;
    uint64_t initial_time;

    uint32_t v_size;
    uint32_t v_pos;

    std::vector<float> color;
   std::vector<uint16_t> retro_color;

    void adjust_screen_params();
    void draw(Chip* chip);
    void draw_overlays();
    void init_color_lut(const double (*r)[3]);
   uint16_t doublergb_to_retrocolor(double r, double g, double b);
   uint16_t retro_blend(uint16_t bg, uint16_t fg);

public:
    const VideoDesc* desc;
    uint32_t frame_count;
    enum VideoPins { HBLANK_PIN = 9, VBLANK_PIN = 10 };
   
   uint16_t *pixel_buf;

   bool frame_done = false;

    
   Video();
     ~Video() { }
     void video_init(int width, int height /*, const Settings::Video& settings */);
     void swap_buffers();
     void show_cursor(bool show);
    static CUSTOM_LOGIC( video );

//    static Video* createDefault(phoenix::VerticalLayout& layout, phoenix::Viewport*& viewport);
};

extern CHIP_DESC( VIDEO );

#endif
