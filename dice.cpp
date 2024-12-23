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

/*
 using phoenix::VerticalLayout;
using phoenix::Viewport;
 */

namespace dice_libretro {

void DICE::init_mem(uint16_t *pixel_buffer)
{
        /*
        input = new Input();
        video = Video::createDefault(layout, viewport);
         */
        input = new Input();
        video = new Video();
        video->pixel_buf = pixel_buffer;
   
        //int game_idx = 0;  // Pong
        int game_idx = 19;
   
        GameDesc& g = game_list[game_idx];
        
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
