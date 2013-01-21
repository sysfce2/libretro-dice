#include "../chip_desc.h"
#include "../circuit.h"

/*
Clock Gate

Speed hack, used to disconnect high frequency signals from a chip 
when they cannot cause a change in the node's chip.

(For example, could be used to disconnect a high frequency signal from an AND gate
when the other input to the AND gate is low)

Input pins:
    1: Clock gate enable signal (low frequency)
    2: Clock or other high frequency signal, to be gated


Output pins: 
    3: Gated clock output

TODO: Find some way to do this automatically?
TODO: Doesn't always work correctly
*/

static CUSTOM_LOGIC( CLK_GATE )
{
    if(mask & 2)
    {
        // Clock input deactivated, so disconnect again
        chip->input_links[1].chip->active_outputs &= ~chip->input_links[1].mask;
    }
    else
    {
        chip->inputs ^= mask;
    }

    if(chip->output_links.size()) // Probably can assume this is true
    {
        Chip* c = chip->output_links[0].chip;
        
        if(chip->inputs & 1)
        {
            // Reconnect to clock
            c->deactivate_outputs();
        }
        else // Disconnect from clock
        {
            c->deactivate_outputs();
            c->state = ACTIVE;
            c->activation_time = c->circuit->global_time;
            c->input_links[0].chip->active_outputs &= ~c->input_links[0].mask;

            c->active_inputs = 0;
            c->cycle_time = 1;
            c->pending_event = 0;
        }
    }
}

static CHIP_LOGIC( CLK_BUF )
{
    pin[3] = pin[2];
}

CHIP_DESC( CLK_GATE ) = 
{
	CUSTOM_CHIP_START( CLK_GATE )
        INPUT_PINS( 1, 2 )
        OUTPUT_PIN( i1 ),

	CHIP_START( CLK_BUF )
        INPUT_PINS( 2, i1 )
        OUTPUT_PIN( 3 )
        OUTPUT_DELAY_NS( 0.0, 0.0 ),

	CHIP_DESC_END
};
