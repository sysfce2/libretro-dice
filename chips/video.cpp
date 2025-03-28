#include "video.h"
#include "../circuit.h"
#include "../libretro.h"
#include "../dice.h"
//#include <phoenix.hpp>

#ifdef __APPLE__
//#include <OpenGL/gl.h> // Think different
#else
//#include <GL/gl.h>
#endif

/*
using phoenix::VerticalLayout;
using phoenix::Viewport;
 */

/* 
	Inputs:
		1-8: VIDEO
		9: HBLANK
		10: VBLANK
	Outputs:
		None
*/

#define HBLANK_MASK (1 << 8)
#define VBLANK_MASK (1 << 9)
#define VIDEO_MASK ((1 << 8) - 1)

extern retro_video_refresh_t video_cb;
extern retro_environment_t environ_cb;
extern retro_log_printf_t log_cb;

// Use buffer to add capacitance to the video output, to smooth out high frequency noise.
// TODO: Is this accurate?
template <unsigned P>
CHIP_LOGIC( VIDEO_Buf ) 
{
    pin[P+10] = pin[P];
}

CHIP_DESC( VIDEO ) = 
{
	// TODO: Change delay values based on input resistance?
    CHIP_START( VIDEO_Buf<1> ) INPUT_PINS(1) OUTPUT_PIN(11) OUTPUT_DELAY_NS( 30.0, 30.0 ),
	CHIP_START( VIDEO_Buf<2> ) INPUT_PINS(2) OUTPUT_PIN(12) OUTPUT_DELAY_NS( 30.0, 30.0 ),
	CHIP_START( VIDEO_Buf<3> ) INPUT_PINS(3) OUTPUT_PIN(13) OUTPUT_DELAY_NS( 30.0, 30.0 ),
	CHIP_START( VIDEO_Buf<4> ) INPUT_PINS(4) OUTPUT_PIN(14) OUTPUT_DELAY_NS( 30.0, 30.0 ),
	CHIP_START( VIDEO_Buf<5> ) INPUT_PINS(5) OUTPUT_PIN(15) OUTPUT_DELAY_NS( 30.0, 30.0 ),
	CHIP_START( VIDEO_Buf<6> ) INPUT_PINS(6) OUTPUT_PIN(16) OUTPUT_DELAY_NS( 30.0, 30.0 ),
	CHIP_START( VIDEO_Buf<7> ) INPUT_PINS(7) OUTPUT_PIN(17) OUTPUT_DELAY_NS( 30.0, 30.0 ),
	CHIP_START( VIDEO_Buf<8> ) INPUT_PINS(8) OUTPUT_PIN(18) OUTPUT_DELAY_NS( 30.0, 30.0 ),
	CHIP_START( VIDEO_Buf<9> ) INPUT_PINS(9) OUTPUT_PIN(19) OUTPUT_DELAY_NS( 30.0, 30.0 ),
	CHIP_START( VIDEO_Buf<10> ) INPUT_PINS(10) OUTPUT_PIN(20) OUTPUT_DELAY_US( 1.0, 1.0 ),

    CUSTOM_CHIP_START(&Video::video)
	    INPUT_PINS(11, 12, 13, 14, 15, 16, 17, 18, 19, 20),
    
    //CUSTOM_CHIP_START(&Video::video)
	//    INPUT_PINS(1, 2, 3, 4, 5, 6, 7, 8, 9, 10),

	CHIP_DESC_END
};

const VideoDesc VideoDesc::DEFAULT = VideoDesc();

Video::Video() : scanline_time(0), current_time(0), initial_time(0), 
    v_size(0), v_pos(0), frame_count(0), desc(&VideoDesc::DEFAULT),
    color(3 << 8),  // Reserve 768 colors for standalone OpenGL LUT
    retro_color(1 << 8) // Reserve 256 for libretro colormap
{ }

void Video::video_init(int width, int height /*, const Settings::Video& settings */)
{
    // Keep aspect ratio
    unsigned x = 0;
    unsigned y = 0;

    /* bool horizontal = true;
    if(desc->orientation == ROTATE_90 || desc->orientation == ROTATE_270)
        horizontal = false;        

    if(settings.keep_aspect && horizontal)
    {
        if(width > 4*height/3)
        {
            x = (width - 4*height/3) / 2;
            width = 4*height/3;
        }
        else if(height > 3*width/4)
        {
            y = (height - 3*width/4) / 2;
            height = 3*width/4;
        }
    }
    else if(settings.keep_aspect) // vertical
    {
        if(width > 3*height/4)
        {
            x = (width - 3*height/4) / 2;
            width = 3*height/4;
        }
        else if(height > 4*width/3)
        {
            y = (height - 4*width/3) / 2;
            height = 4*width/3;
        }
    }
     */

   	//glViewport(x, y, width, height);

    adjust_screen_params();
}

void Video::adjust_screen_params()
{
    /*
     glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

    switch(desc->orientation)
    {
        case ROTATE_90: glRotatef(90.0, 0.0, 0.0, -1.0); break;
        case ROTATE_180: glRotatef(180.0, 0.0, 0.0, -1.0); break;
        case ROTATE_270: glRotatef(270.0, 0.0, 0.0, -1.0); break;
        default: break;
    }
     
    glOrtho(0.0, scanline_time, v_size, 0.0, -1.0, 1.0);
*/
   bool horizontal = true;
   if(desc->orientation == ROTATE_90 || desc->orientation == ROTATE_270)
       horizontal = false;

   // If game uses a vertical monitor, tell libretro and adjust aspect ratio.
   VideoOrientation orientation = desc->orientation;
   environ_cb(RETRO_ENVIRONMENT_SET_ROTATION, &orientation);

   if (v_size > 0)
   {
      struct retro_system_av_info avinfo;
      retro_get_system_av_info(&avinfo);
      log_cb(RETRO_LOG_DEBUG, "avinfo_max_height %d\n", avinfo.geometry.max_height);
      avinfo.geometry.base_height = v_size;
      avinfo.geometry.aspect_ratio = horizontal ? 4.0f / 3.0f : 3.0f / 4.0f;
      
      /*if (avinfo.geometry.max_height != desc->retro_v_size)
      {
         // Set maximum height based on circuit description.  Heavy.
         avinfo.geometry.max_height = desc->retro_v_size;
         environ_cb(RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO, &avinfo);
      } else { */
         // Just set current height based on emulation.  Light.
         environ_cb(RETRO_ENVIRONMENT_SET_GEOMETRY, &avinfo);
         log_cb(RETRO_LOG_INFO, "set_geometry v_size %d\n", v_size);
      /* } */
   }

   /*
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    // Clear screen
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	swap_buffers();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    */
    if(desc->monitor_type == COLOR)
    {
        init_color_lut(desc->r_color);
    }
    else
    {
        VIDEO_RESISTOR_MATRIX temp;
        for(int i = 0; i < 8; i++)
            temp[i][0] = temp[i][1] = temp[i][2] = desc->r[i];

        init_color_lut(temp);
    } 
}

void Video::init_color_lut(const double (*r)[3])
{
    // Initialize color array
    for(int i = 3; i < color.size(); i++)
    {
        int v = i / 3;
        int c = i % 3;
        
        double r_hi = 0.0, r_lo = 0.0;

        for(int j = 0; j < 8; j++)
        {
            if(r[j][c] < 1.0) continue; // Assume 1 Ohm minimum

            if(v & (1 << j))
                r_hi += 1.0 / r[j][c];
            else
                r_lo += 1.0 / r[j][c];
        }

        double val;
        if(r_lo < 1.0e-6)
        {
            val = 1.0;
        }
        else if(r_hi < 1.0e-6)
        {
            val = 0.0;
        }
        else
        {
            r_hi = 1.0 / r_hi;
            r_lo = 1.0 / r_lo;
            val = r_lo / (r_hi + r_lo);
        }

        color[i] = (val + desc->brightness) * desc->contrast; // TODO: user configurable brightness/contrast?
        //printf("Color %d: %g %g %g %g\n", i, r_lo, r_hi, val, color[i]);
    }
   
   // Boil the old OpenGL Look Up Table down to a libretro-ish RGB565 colormap.
   for (int i = 0; i < color.size() / 3; i++) {
      retro_color[i] = doublergb_to_retrocolor(color[i*3], color[i*3+1], color[i*3+2]);
   }
}

uint16_t Video::doublergb_to_retrocolor(double r, double g, double b)
{
   // LUT isn't clamped to 1.0, do that here so we don't wrap around.
   uint16_t red5 = fmin(r,1) * 31;
   uint16_t green6 = fmin(g,1) * 63;
   uint16_t blue5 = fmin(b,1) * 31;
   return red5 << 11 | green6 << 5 | blue5;
}

void Video::draw(Chip* chip)
{
    uint64_t start_time = current_time - initial_time;
    uint64_t end_time = chip->circuit->global_time - initial_time;

   uint64_t MAX_SCANLINE_TIME = 52214160;
   //float ratio = float(VIDEO_WIDTH) / float(MAX_SCANLINE_TIME);
   float ratio = float(VIDEO_WIDTH) / float(scanline_time);
   //uint16_t c = 0xffff; // (chip->inputs & VIDEO_MASK) * 1000;
   //uint16_t c = (chip->inputs & VIDEO_MASK) * 0x3333; // TODO (mittonk): Index by 3, don't multiply.
   //uint16_t c = bool(chip->inputs & VIDEO_MASK) ? 0xffff : 0x0;
   uint16_t c = retro_color[(chip->inputs & VIDEO_MASK)];

   if((chip->inputs & VIDEO_MASK) || desc->scan_mode == INTERLACED) // Falling edge
    {
        //float* c_opengl = &color[(chip->inputs & VIDEO_MASK) * 3];

       uint64_t left = float(start_time) * ratio;
       uint64_t right = fmin(float(end_time) * ratio, VIDEO_WIDTH);
       for (uint64_t i = left; i < right; i++) {
          pixel_buf[i+v_pos*VIDEO_WIDTH] = c;
       }
        /*glBegin(GL_QUADS);
            glColor3fv(c);

	        glVertex3f(start_time, v_pos,     0.0);
	        glVertex3f(end_time,   v_pos,     0.0);
	        glVertex3f(end_time,   v_pos+1.0, 0.0);
	        glVertex3f(start_time, v_pos+1.0, 0.0);
	    glEnd(); */
    }
}

void Video::draw_overlays()
{
   
    if(desc->overlays.empty()) return;

    /*glEnable(GL_BLEND);
    glBlendFunc(GL_ZERO, GL_SRC_COLOR); */

    for(const VideoOverlay& o : desc->overlays)
    {
     /*   double end_x = (o.width < 0.0) ? scanline_time : (o.x + o.width) / Circuit::timescale;
        double end_y = (o.height < 0.0) ? v_size : o.y + o.height;
        double start_x = o.x / Circuit::timescale;
        double start_y = o.y;

        glBegin(GL_QUADS);
            glColor3f(o.r, o.g, o.b);

	        glVertex3f(start_x, start_y, 0.0);
	        glVertex3f(end_x,   start_y, 0.0);
	        glVertex3f(end_x,   end_y,   0.0);
	        glVertex3f(start_x, end_y,   0.0);
	    glEnd(); */

       bool horizontal = true;
       if(desc->orientation == ROTATE_90 || desc->orientation == ROTATE_270)
           horizontal = false;

       // Negative values mean "full screen".
       int left, top, right, bottom;
       if (horizontal)
       {
          left = o.x;
          top = o.y;
          right = (o.width < 0.0) ? VIDEO_WIDTH : o.x + o.width;
          bottom = (o.height < 0.0) ? VIDEO_HEIGHT : o.y + o.height;
       } else {
          // Good enough for Breakout.
          // TODO (mittonk): Fix to handle another hypothetical vertical overlay.
          left = fmin(o.x / Circuit::timescale / scanline_time * VIDEO_WIDTH, VIDEO_WIDTH);
          top = o.x;
          right = fmin((o.x + o.width) / Circuit::timescale / scanline_time * VIDEO_WIDTH, VIDEO_WIDTH);
          bottom = (o.height < 0.0) ? VIDEO_HEIGHT : o.y + o.height;
       }
       uint16_t c = doublergb_to_retrocolor(o.r, o.g, o.b);
       for (int i = left; i < right; i++)
       {
          for (int j = top; j < bottom; j++)
          {
             uint16_t existing = pixel_buf[i + j*VIDEO_WIDTH];
             //pixel_buf[i + j*VIDEO_WIDTH] = c;
             pixel_buf[i + j*VIDEO_WIDTH] = retro_blend(existing, c);
          }
       }

    }

    /*glDisable(GL_BLEND); */
}

uint16_t Video::retro_blend(uint16_t bg, uint16_t fg)
{
   // TODO (mittonk): Something closer to glBlendFunc(GL_ZERO, GL_SRC_COLOR).
   // This is recognizable for a monochrome background and overlay foreground, though.
   return bg & fg;
}

CUSTOM_LOGIC( Video::video )
{
    Video* video = (Video*)chip->custom_data;
    uint64_t global_time = chip->circuit->global_time;

    // VBLANK rising edge, draw frame
    if(mask == VBLANK_MASK && !(chip->inputs & VBLANK_MASK))
    {
        if(video->desc->scan_mode == INTERLACED)
            video->v_pos += ~video->v_pos & 1; // Round up to odd number
        
        if(video->v_pos != video->v_size)
        {
            video->v_size = video->v_pos;
            video->adjust_screen_params();
        }
        video->draw_overlays();
        video->swap_buffers();
        /*if(video->desc->scan_mode == PROGRESSIVE)
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
         */
        video->frame_count++;
        
        // Make sure real time is caught up
       
       // if(chip->circuit->settings.throttle)
       if (true)
            while(chip->circuit->rtc.get_usecs() < uint64_t(global_time * 1000000.0 * Circuit::timescale));
        
       
    }
    // HBLANK rising edge, go to next line
    else if(mask == HBLANK_MASK && !(chip->inputs & HBLANK_MASK)) 
    {
        // Skip length check on first/last line and during vblank, since some games
        // use a different horizontal count on the first or last scanline
        if(video->v_pos > 0 && video->v_pos < video->v_size && (global_time - video->initial_time) != video->scanline_time)
        {
            log_cb(RETRO_LOG_INFO, "Adjust screen params old:%lld new:%lld pos:%d\n", video->scanline_time, global_time - video->initial_time, video->v_pos);
            video->scanline_time = global_time - video->initial_time;
            video->adjust_screen_params();
        }

        video->draw(chip);

        if(video->desc->scan_mode == INTERLACED)
            video->v_pos += 2;
        else
            video->v_pos++;
    }
    // VBLANK falling edge, set vblank start pos
    else if(mask == VBLANK_MASK && (chip->inputs & VBLANK_MASK))
    {
        // Detect odd or even field. TODO: Is this accurate?
        if(video->desc->scan_mode == INTERLACED && (global_time - video->initial_time > video->scanline_time))
            video->v_pos = 1;
        else
            video->v_pos = 0;
    }
    // HBLANK falling edge, set initial time
    else if(mask == HBLANK_MASK && (chip->inputs & HBLANK_MASK))
    {
        video->initial_time = global_time;
    }
    // Draw video
    else if(!(chip->inputs & (HBLANK_MASK|VBLANK_MASK)))
    {
        video->draw(chip);     
    }

    chip->inputs ^= mask;
    video->current_time = global_time;
     
}

void Video::swap_buffers() {
   // Copy to external-facing frame buffer.
   for (unsigned i = 0; i<VIDEO_PIXELS; i++) {
      retro_pixel_buf[i] = pixel_buf[i];
   }

   // Display.
   video_cb(reinterpret_cast<uint8_t*>(retro_pixel_buf), VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_PITCH);
   
   // Wipe the frame buffer we just displayed.
   for (unsigned i = 0; i<VIDEO_PIXELS; i++) {
      pixel_buf[i] = 0;
   }

   frame_done = true;
}
