#include "../chip_desc.h"

/*
7493
4-bit asynchronous binary counter with /2 and /8 sections and reset.

      +---+--+---+
/CLK1 |1  +--+ 14| /CLK0
 RST1 |2       13|
 RST2 |3       12| Q0
      |4  7493 11| Q3
  VCC |5       10| GND
      |6        9| Q1
      |7        8| Q2
      +----------+
*/

/* TODO:
 * Verify output delays
   1972 Fairchild databook says 75 ns typical clkA->qD, but has no info in individual delays,
   1976 TI databook says 46 ns typical, but this is for 7493A. Delay this short messes up pong net circuit.

 * Add NAND gate for more accurate reset delay?
 */

static CHIP_LOGIC( 7493A )
{
	if(pin[2] && pin[3])
		pin[12] = 0;
	else if(prev_pin[14] && !pin[14])
		pin[12] = prev_pin[12] ^ 1;
	else
		pin[12] = prev_pin[12];
}

static CHIP_LOGIC( 7493B )
{
	if(pin[2] && pin[3])
		pin[9] = 0;
	else if(prev_pin[1] && !pin[1])
		pin[9] = prev_pin[9] ^ 1;
	else
		pin[9] = prev_pin[9];
}

static CHIP_LOGIC( 7493C )
{
	if(pin[2] && pin[3])
		pin[8] = 0;
	else if(prev_pin[9] && !pin[9])
		pin[8] = prev_pin[8] ^ 1;
	else
		pin[8] = prev_pin[8];
}

static CHIP_LOGIC( 7493D )
{
	if(pin[2] && pin[3])
		pin[11] = 0;
	else if(prev_pin[8] && !pin[8])
		pin[11] = prev_pin[11] ^ 1;
	else
		pin[11] = prev_pin[11];
}


CHIP_DESC( 7493 ) =
{
	CHIP_START( 7493A )
        INPUT_PINS( 2, 3, 14 )
        OUTPUT_PIN( 12 )
		PREV_INPUT_PIN( 14 )
		PREV_OUTPUT_PIN( 12 )
        OUTPUT_DELAY_NS( 17.0, 19.0 ),

	CHIP_START( 7493B )
        INPUT_PINS( 2, 3, 1 )
        OUTPUT_PIN( 9 )
		PREV_INPUT_PIN( 1 )
		PREV_OUTPUT_PIN( 9 )
        OUTPUT_DELAY_NS( 17.0, 19.0 ),

	CHIP_START( 7493C )
        INPUT_PINS( 2, 3, 9 )
        OUTPUT_PIN( 8 )
		PREV_INPUT_PIN( 9 )
		PREV_OUTPUT_PIN( 8 )
        OUTPUT_DELAY_NS( 17.0, 19.0 ),

	CHIP_START( 7493D )
        INPUT_PINS( 2, 3, 8 )
        OUTPUT_PIN( 11 )
		PREV_INPUT_PIN( 8 )
		PREV_OUTPUT_PIN( 11 )
        OUTPUT_DELAY_NS( 17.0, 19.0 ),

    CHIP_DESC_END
};

CHIP_DESC( 7493A ) =
{
	CHIP_START( 7493A )
        INPUT_PINS( 2, 3, 14 )
        OUTPUT_PIN( 12 )
		PREV_INPUT_PIN( 14 )
		PREV_OUTPUT_PIN( 12 )
        OUTPUT_DELAY_NS( 10.0, 12.0 ),

	CHIP_START( 7493B )
        INPUT_PINS( 2, 3, 1 )
        OUTPUT_PIN( 9 )
		PREV_INPUT_PIN( 1 )
		PREV_OUTPUT_PIN( 9 )
        OUTPUT_DELAY_NS( 10.0, 14.0 ),

	CHIP_START( 7493C )
        INPUT_PINS( 2, 3, 9 )
        OUTPUT_PIN( 8 )
		PREV_INPUT_PIN( 9 )
		PREV_OUTPUT_PIN( 8 )
        OUTPUT_DELAY_NS( 11.0, 9.0 ),

	CHIP_START( 7493D )
        INPUT_PINS( 2, 3, 8 )
        OUTPUT_PIN( 11 )
		PREV_INPUT_PIN( 8 )
		PREV_OUTPUT_PIN( 11 )
        OUTPUT_DELAY_NS( 13.0, 9.0 ),

    CHIP_DESC_END
};


