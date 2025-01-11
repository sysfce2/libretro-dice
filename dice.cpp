#include <iostream>
#include <fstream>
#include <regex>
#include <stdexcept>
#include <string>

//#include <phoenix.hpp>
#include "dice.h"

#include "circuit.h"
#include "circuit_desc.h"

#include "game_list.h"
#include "chips/rom.h"
#include "filename.h"

/*
 using phoenix::VerticalLayout;
using phoenix::Viewport;
 */

namespace dice_libretro {

void DICE::load_game(const char *path, uint16_t *pixel_buf1, uint16_t *pixel_buf2, bool *write_to_frame_buf1)
{
        /*
        input = new Input();
        video = Video::createDefault(layout, viewport);
         */
        input = new Input();
        video = new Video();
        video->pixel_buf1 = pixel_buf1;
        video->pixel_buf2 = pixel_buf2;
        video->write_to_frame_buf1 = write_to_frame_buf1;
   
        video->video_init(VIDEO_WIDTH, VIDEO_HEIGHT);

        const string extension = nall::extension(path);
        const string basename = nall::basename(nall::notdir(path));
        RomDesc::set_zip_filename(path);
        int i = 0;
        for (i = 0; i < game_list_size; i++) {
           if (game_list[i].command_line == basename)
           {
              break;
           }
        }
        // TODO (mittonk): Error out if game not found.
   
        GameDesc& g = game_list[i];
        
        circuit = new Circuit(settings,
                              *input,
                              *video,
                              g.desc, g.command_line);
}

void DICE::run(void)
{
    // Run until the video frame is finished.
    if(circuit)
    {
       // Run until we've got a full video frame, but not much more.
       while (!circuit->video.request_video_callback) {
            // circuit->run(2.5e-3 / Circuit::timescale); // Run 2.5 ms
          circuit->run(1.0e-3 / Circuit::timescale); // Run 1 ms
       }
       // Headed out to video callback, clear request.
       circuit->video.request_video_callback = false;
    }
}

void DICE::render_frame(void)
{
}

void DICE::update_input(int32_t input_state[], int32_t input_analog_left_x[], int32_t input_analog_left_y[])
{
   if (circuit)
   {
      for (unsigned i=0; i<NUM_CONTROLLERS; i++)
      {
         circuit->input.input_state[i] = input_state[i];
         circuit->input.input_analog_left_x[i] = input_analog_left_x[i];
         circuit->input.input_analog_left_y[i] = input_analog_left_y[i];
      }
   }
}

void DICE::reset(void)
{
}

} // namespace
