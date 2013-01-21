#include "capacitor.h"
#include "../circuit.h"

/* 
Parallel Capacitor

Used to add extra delay to a TTL output.
Must be placed in series with the input signal, to act as a buffer.
Additionally supports an optional series resistance before the capacitor.

Input: Pin 1
Output: Pin 2

        R
    (optional)
1 ----/\/\/----+---- 2
               |
            C ---
              ---
               |
               |
              ---
               -

Assuming 130 Ohm drive resistance on high output (taken from 7400 series datasheet),
And that the capacitor needs to charge to 58.8% (due to minimum high input voltage = 2 V, 
typical high output voltage = 3.4 V).

Assuming discharge time is the same. TODO: Figure out accurate discharge time calculation.
7400 databook seems to imply that the discharge time would be the same as (or slightly longer
than) the charge time, despite much lower resistance to ground. This could be due to the
transistor to VCC only shutting off once the capacitor voltage drops below a certain level?
*/

static const double TIME_CONSTANT = -log((3.4 - 2.0) / 3.4);

CUSTOM_LOGIC( CapacitorDesc::capacitor )
{
    CapacitorDesc* desc = (CapacitorDesc*)chip->custom_data;

    // Update tp_lh, tp_hl in standard portion based on RC value
    if(chip->output_links.size())
    {
        chip->output_links[0].chip->delay[0] = chip->output_links[0].chip->delay[1] =
            uint64_t(TIME_CONSTANT * (130.0 + desc->r) * desc->c / Circuit::timescale);
    }
}

static CHIP_LOGIC( cap )
{
    pin[2] = pin[1];
}

CHIP_DESC( CAPACITOR ) = 
{
	CUSTOM_CHIP_START(&CapacitorDesc::capacitor)
        OUTPUT_PIN( i1 ),

	CHIP_START( cap )
        INPUT_PINS( 1, i1 )
        OUTPUT_PIN( 2 )
        OUTPUT_DELAY_NS( 50.0, 50.0 ), // tp_lh, tp_hl will be overwritten
 
	CHIP_DESC_END
};





/* 
Series Capacitor/Resistor circuit

Used to create timed pulse on the rising or falling edge of a signal.

Inputs: Pin 1 (Capacitor connection)
        Pin 2 (Resistor connection, should probably be VCC or GND)
Output: Pin 3

        C
 1 ----||----+---- 3
             |
             >
             < R
             >
             |
            
             2

TODO: Determine exact timing? Depends on the exact voltage level at which a TTL chip would detect a transition.
TODO: Output can get stuck in high state if small input pulse occurs? (When used as a monostable timer)
*/

CUSTOM_LOGIC( SeriesRCDesc::series_rc )
{
    SeriesRCDesc* desc = (SeriesRCDesc*)chip->custom_data;

    // Update tp_lh, tp_hl in standard portion based on rc value
    if(chip->output_links.size())
    {
        chip->output_links[0].chip->delay[1] = uint64_t(desc->r * desc->c / Circuit::timescale);
    }
}

CHIP_LOGIC( series_rc ) // > 0 rise / fall time
{
    if(pin[1] != pin[2] && prev_pin[1] == pin[2])
        pin[3] = pin[1];
    else if(pin[1] == pin[2] || !pin[i2])
        pin[3] = pin[2];
    else
        pin[3] = prev_pin[3];
}

CHIP_LOGIC( series_cap ) // 0 rise time, long fall time
{
    if(pin[1] != pin[2] && prev_pin[1] == pin[2])
        pin[i2] = 1;
    else if(pin[3] != pin[2])
        pin[i2] = 0;
    else
        pin[i2] = prev_pin[i2];
}

CHIP_DESC( SERIES_RC ) = 
{
	CUSTOM_CHIP_START(&SeriesRCDesc::series_rc)
        OUTPUT_PIN( i1 ),

	CHIP_START( series_rc )
        INPUT_PINS( 1, 2, i2 )
        PREV_INPUT_PIN( 1 )
        OUTPUT_PIN( 3 )
        PREV_OUTPUT_PIN( 3 )
        OUTPUT_DELAY_NS( 5.0, 5.0 ),

	CHIP_START( series_cap )
        INPUT_PINS( i1, 1, 2, 3 )
        PREV_INPUT_PIN( 1 )
        OUTPUT_PIN( i2 )
        PREV_OUTPUT_PIN( i2 )
        OUTPUT_DELAY_NS( 0.0, 1.0 ), // tp_hl will be overwritten

	CHIP_DESC_END
};





/* 
Buffer

Generic buffer. Used to hack extra delay into the circuit
when additional delay is inexplicably needed.

Input: Pin 1
Output: Pin 2
*/

CUSTOM_LOGIC( BufferDesc::buffer )
{
    BufferDesc* desc = (BufferDesc*)chip->custom_data;

    // Update tp_lh, tp_hl in standard portion based on RC value
    if(chip->output_links.size())
    {
        chip->output_links[0].chip->delay[0] = uint64_t(desc->tp_lh / Circuit::timescale);
        chip->output_links[0].chip->delay[1] = uint64_t(desc->tp_hl / Circuit::timescale);
    }
}

static CHIP_LOGIC( buffer )
{
    pin[2] = pin[1];
}

CHIP_DESC( BUFFER ) = 
{
	CUSTOM_CHIP_START(&BufferDesc::buffer)
        OUTPUT_PIN( i1 ),

	CHIP_START( buffer )
        INPUT_PINS( 1, i1 )
        OUTPUT_PIN( 2 )
        OUTPUT_DELAY_NS( 50.0, 50.0 ), // tp_lh, tp_hl will be overwritten
 
	CHIP_DESC_END
};


