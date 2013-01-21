#include "../chip_desc.h"
#include "../circuit.h"

/* 
	Clock

Inputs: None
Clock Output: Pin 1

*/

CUSTOM_LOGIC( clock )
{
    chip->state = ACTIVE;
    chip->activation_time = chip->circuit->global_time;

    chip->output_events.resize(2);
    chip->output_events[0] = chip->delay[0];
    chip->output_events[1] = chip->delay[1];
    chip->cycle_time = chip->delay[0] + chip->delay[1];
    chip->current_output_event = 0;
    
    chip->pending_event = chip->circuit->queue_push(chip, chip->delay[0]);
}

CHIP_DESC( CLOCK_14_318_MHZ ) = 
{
	CUSTOM_CHIP_START(&clock)
        OUTPUT_PIN( 1 )
        OUTPUT_DELAY_US( 0.5/14.31818, 0.5/14.31818 ),

	CHIP_DESC_END
};

CHIP_DESC( CLOCK_6_MHZ ) = 
{
	CUSTOM_CHIP_START(&clock)
        OUTPUT_PIN( 1 )
        OUTPUT_DELAY_US( 0.5/6.0, 0.5/6.0 ),

	CHIP_DESC_END
};
