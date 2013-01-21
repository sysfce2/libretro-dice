#include "555astable.h"
#include "../circuit.h"

/*
555
Universal timer (Astable mode).
When CV is not connected it is held at 2/3.VCC through a 5k/10k divider.

     +---+--+---+            +------+--------+------*---+-----+
 GND |1  +--+  8| VCC        | /RST |  /TR   | Thr  | Q | Dis |
 /TR |2        7| Dis        +======+========+======*===+=====+
   Q |3   555  6| Thr        |  0   |   X    |  X   | 0 |  0  |
/RST |4        5| CV         |  1   | < CV/2 |  X   | 1 |  Z  |
     +----------+            |  1   | > CV/2 | < CV | - |  -  |
                             |  1   | > CV/2 | > CV | 0 |  0  |
                             +------+--------+------*---+-----+
*/

const double Astable555Desc::LN_2 = 0.69314718056;

CUSTOM_LOGIC( Astable555Desc::astable_555 )
{
    Astable555Desc* desc = (Astable555Desc*)chip->custom_data;

    if(chip->state == ACTIVE)
        chip->deactivate_outputs();

    chip->inputs ^= mask;

    // Update tp_lh, tp_hl in standard portion based on r and c values
    if(mask == 1 && !(chip->inputs & 1)) // Negedge /RST (reset being asserted)
    {
        if(chip->output)
            chip->pending_event = chip->circuit->queue_push(chip, 100.0e-9 / Circuit::timescale);
    }
    else if(chip->inputs & 1) // Posedge /RST (reset being de-asserted)
    {
        uint64_t hi_time = uint64_t(LN_2 * (desc->r1 + desc->r2) * desc->c / Circuit::timescale);
        uint64_t lo_time = uint64_t(LN_2 * desc->r2 * desc->c / Circuit::timescale);

        chip->state = ACTIVE;
        chip->output_events.resize(2);
        chip->cycle_time = hi_time + lo_time;
        chip->activation_time = chip->circuit->global_time;

        if(chip->output)
        {
            chip->output_events[0] = hi_time;
            chip->output_events[1] = lo_time;
            chip->current_output_event = 1;
            chip->pending_event = chip->circuit->queue_push(chip, hi_time);
        }
        else
        {
            chip->output_events[0] = lo_time;
            chip->output_events[1] = hi_time;
            chip->current_output_event = 1;
            chip->pending_event = chip->circuit->queue_push(chip, lo_time);
        }
    }
}

CHIP_DESC( 555_Astable ) = 
{
	CUSTOM_CHIP_START(&Astable555Desc::astable_555)
        INPUT_PINS( 4, i3 )
        OUTPUT_PIN( 3 )
        OUTPUT_DELAY_NS( 50.0, 50.0 ),

	CHIP_DESC_END
};
