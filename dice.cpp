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
        /* viewport = new Viewport();
        layout.append(*viewport, {~0, ~0});
        //TODO (mittonk): append(layout);

        input = new Input();
        video = Video::createDefault(layout, viewport);
         */
        video = new Video();
        video->pixel_buf = pixel_buffer;
   
        int game_idx = 0;  // Pong
        // int game_idx = 7;  // Breakout

        GameDesc& g = game_list[game_idx];
        
        circuit = new Circuit(settings,
                              //*input,
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

void DICE::update_input(int32_t *input_state)
{
}

void DICE::reset(void)
{
}

} // namespace
