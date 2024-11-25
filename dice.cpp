#include <iostream>
#include <fstream>
#include <regex>
#include <stdexcept>
#include <string>

#include <phoenix.hpp>
#include "dice.h"

#include "circuit.h"
#include "circuit_desc.h"

#include "game_list.h"

using phoenix::VerticalLayout;
using phoenix::Viewport;

namespace dice_libretro {

void DICE::init_mem(void)
{
        viewport = new Viewport();
        layout.append(*viewport, {~0, ~0});
        //TODO (mittonk): append(layout);

        input = new Input();
        video = Video::createDefault(layout, viewport);

        int game_idx = 0;  // Pong
        GameDesc& g = game_list[game_idx];
        
        circuit = new Circuit(settings, *input, *video, g.desc, g.command_line);
}

void DICE::run(void)
{
    if(circuit)
    {
       circuit->run(2.5e-3 / Circuit::timescale); // Run 2.5 ms
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
