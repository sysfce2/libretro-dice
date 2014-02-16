#include "../circuit.h"
#include "2533.h"

/*
2533
1024-Bit Static Shift Register

       +--------+
   OUT |1      8| Vcc
   Vgg |2  25  7| IN2
   SEL |3  33  6| CLK
   GND |4      5| IN1
       +--------+
*/

enum { SEL_MASK = 1, IN1_MASK = 2, CLK_MASK = 4, IN2_MASK = 8 }; 

// TODO: Add buffers to handle setup / hold times?

static CUSTOM_LOGIC( RAM_2533 )
{
    if(chip->custom_data == NULL)
    {
        printf("ERROR: Missing Ram2533Desc for chip %p\n", chip);
        return;
    }

    Ram2533Desc* desc = (Ram2533Desc*)chip->custom_data;

    chip->state = PASSIVE;
    chip->inputs ^= mask;

    int in_data = (chip->inputs & SEL_MASK) ? ((chip->inputs >> 3) & 1) : ((chip->inputs >> 1) & 1);
    int out_addr = (desc->addr + 1) & 1023;

    if(chip->inputs & CLK_MASK) // CLK high, enter data
    {
        desc->data[desc->addr] = in_data;
    }
    else if(mask & CLK_MASK) // CLK falling edge, shift data
    {
        desc->addr = out_addr;
        out_addr = (out_addr + 1) & 1023;
    }

    int new_out = desc->data[out_addr];

    if(new_out != chip->output && chip->pending_event == 0)
    {
        // Schedule event
        chip->pending_event = chip->circuit->queue_push(chip, chip->delay[chip->output]);
    }
    else if(chip->pending_event && new_out == chip->output)
    {
        // Cancel event
        chip->pending_event = 0;
    }
}

CHIP_DESC( 2533 ) = 
{
	CUSTOM_CHIP_START( &RAM_2533 )
	   INPUT_PINS( 3, 5, 6, 7 )
	   OUTPUT_PIN( 1 )
	   OUTPUT_DELAY_NS( 200.0, 200.0 ),

   	CHIP_DESC_END
};

