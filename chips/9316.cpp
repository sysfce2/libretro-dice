#include "../chip_desc.h"

/*
74161 (9316)
4-bit synchronous binary counter with load, asynchronous reset, and ripple carry output.

     +---+--+---+
/RST |1  +--+ 16| VCC
 CLK |2       15| RCO
  P0 |3       14| Q0
  P1 |4   74  13| Q1
  P2 |5  161  12| Q2
  P3 |6       11| Q3
 ENP |7       10| ENT
 GND |8        9| /LOAD
     +----------+
*/

#if 1

static CHIP_LOGIC( 9316_CLK )
{
    if(pin[2] && !prev_pin[2] && (!pin[9] || (pin[7] && pin[10])))
        pin[i1] = 1;
    else if(!pin[2] && prev_pin[2])
        pin[i1] = 0;
    else
        pin[i1] = prev_pin[i1];
}


static CHIP_LOGIC( 9316A )
{
	if(!pin[1])
		pin[14] = 0;
	else if(pin[i1] && !prev_pin[i1] && !pin[9])
		pin[14] = pin[3];
    else if(pin[i1] && !prev_pin[i1])
		pin[14] = prev_pin[14] ^ 1;
    else
        pin[14] = prev_pin[14];
}


static CHIP_LOGIC( 9316B )
{
	if(!pin[1])
		pin[13] = 0;
	else if(pin[i1] && !prev_pin[i1] && !pin[9])
		pin[13] = pin[4];
    else if(pin[i1] && !prev_pin[i1])
		pin[13] = prev_pin[13] ^ pin[14];
    else
        pin[13] = prev_pin[13];
}

static CHIP_LOGIC( 9316C )
{
	if(!pin[1])
		pin[12] = 0;
	else if(pin[i1] && !prev_pin[i1] && !pin[9])
		pin[12] = pin[5];
    else if(pin[i1] && !prev_pin[i1])
		pin[12] = (pin[14] & pin[13]) ^ prev_pin[12];
    else
        pin[12] = prev_pin[12];
}

static CHIP_LOGIC( 9316D )
{
	if(!pin[1])
		pin[11] = 0;
	else if(pin[i1] && !prev_pin[i1] && !pin[9])
		pin[11] = pin[6];
    else if(pin[i1] && !prev_pin[i1])
		pin[11] = (pin[14] & pin[13] & pin[12]) ^ prev_pin[11];
    else
        pin[11] = prev_pin[11];
}

static CHIP_LOGIC( 9316_TC )
{
    pin[15] = pin[10] & pin[11] & pin[12] & pin[13] & pin[14];
}


CHIP_DESC( 9316 ) =
{
    CHIP_START( 9316_CLK )
        INPUT_PINS( 2, 7, 9, 10 )
        OUTPUT_PIN( i1 )
        PREV_INPUT_PIN( 2 )
        PREV_OUTPUT_PIN( i1 )
        OUTPUT_DELAY_NS( 0.01, 0.01 ),

    CHIP_START( 9316A )
        INPUT_PINS( 1, 3, 9, i1 )
        OUTPUT_PIN( 14 )
        PREV_INPUT_PIN( i1 )
		PREV_OUTPUT_PIN( 14 )
        OUTPUT_DELAY_NS( 12.0, 15.0 ),

	CHIP_START( 9316B )
        INPUT_PINS( 1, 4, 9, 14, i1 )
        OUTPUT_PIN( 13 )
        PREV_INPUT_PIN( i1 )
		PREV_OUTPUT_PIN( 13 )
        OUTPUT_DELAY_NS( 12.0, 15.0 ),

	CHIP_START( 9316C )
        INPUT_PINS( 1, 5, 9, 14, 13, i1 )
        OUTPUT_PIN( 12 )
        PREV_INPUT_PIN( i1 )
		PREV_OUTPUT_PIN( 12 )
        OUTPUT_DELAY_NS( 12.0, 15.0 ),

	CHIP_START( 9316D )
        INPUT_PINS( 1, 6, 9, 14, 13, 12, i1 )
        OUTPUT_PIN( 11 )
        PREV_INPUT_PIN( i1 )
		PREV_OUTPUT_PIN( 11 )
        OUTPUT_DELAY_NS( 12.0, 15.0 ),

    CHIP_START( 9316_TC )
        INPUT_PINS( 10, 14, 13, 12, 11)
        OUTPUT_PIN( 15 )
        OUTPUT_DELAY_NS( 6.0, 2.0 ), // Somewhat arbitrary

    CHIP_DESC_END
};


#else
static CHIP_LOGIC( 9316A )
{
	if(!pin[1])
		pin[14] = 0;
	else if(pin[2] && !prev_pin[2] && !pin[9])
		pin[14] = pin[3];
    else if(pin[2] && !prev_pin[2] && pin[7] && pin[10])
		pin[14] = prev_pin[14] ^ 1;
    else
        pin[14] = prev_pin[14];
}


static CHIP_LOGIC( 9316B )
{
	if(!pin[1])
		pin[13] = 0;
	else if(pin[2] && !prev_pin[2] && !pin[9])
		pin[13] = pin[4];
    else if(pin[2] && !prev_pin[2] && pin[7] && pin[10])
		pin[13] = prev_pin[13] ^ pin[14];
    else
        pin[13] = prev_pin[13];
}

static CHIP_LOGIC( 9316C )
{
	if(!pin[1])
		pin[12] = 0;
	else if(pin[2] && !prev_pin[2] && !pin[9])
		pin[12] = pin[5];
    else if(pin[2] && !prev_pin[2] && pin[7] && pin[10])
		pin[12] = (pin[14] & pin[13]) ^ prev_pin[12];
    else
        pin[12] = prev_pin[12];
}

static CHIP_LOGIC( 9316D )
{
	if(!pin[1])
		pin[11] = 0;
	else if(pin[2] && !prev_pin[2] && !pin[9])
		pin[11] = pin[6];
    else if(pin[2] && !prev_pin[2] && pin[7] && pin[10])
		pin[11] = (pin[14] & pin[13] & pin[12]) ^ prev_pin[11];
    else
        pin[11] = prev_pin[11];
}

static CHIP_LOGIC( 9316_TC )
{
    pin[15] = pin[10] & pin[11] & pin[12] & pin[13] & pin[14];
}

CHIP_DESC( 9316 ) =
{
	CHIP_START( 9316A )
        INPUT_PINS( 1, 2, 3, 7, 9, 10 )
        OUTPUT_PIN( 14 )
		PREV_INPUT_PIN( 2 )
		PREV_OUTPUT_PIN( 14 )
        OUTPUT_DELAY_NS( 12.0, 15.0 ),

	CHIP_START( 9316B )
        INPUT_PINS( 1, 2, 4, 7, 9, 10, 14 )
        OUTPUT_PIN( 13 )
		PREV_INPUT_PIN( 2 )
		PREV_OUTPUT_PIN( 13 )
        OUTPUT_DELAY_NS( 12.0, 15.0 ),

	CHIP_START( 9316C )
        INPUT_PINS( 1, 2, 5, 7, 9, 10, 14, 13 )
        OUTPUT_PIN( 12 )
		PREV_INPUT_PIN( 2 )
		PREV_OUTPUT_PIN( 12 )
        OUTPUT_DELAY_NS( 12.0, 15.0 ),

	CHIP_START( 9316D )
        INPUT_PINS( 1, 2, 6, 7, 9, 10, 14, 13, 12 )
        OUTPUT_PIN( 11 )
		PREV_INPUT_PIN( 2 )
		PREV_OUTPUT_PIN( 11 )
        OUTPUT_DELAY_NS( 12.0, 15.0 ),

    CHIP_START( 9316_TC )
        INPUT_PINS( 10, 14, 13, 12, 11)
        OUTPUT_PIN( 15 )
        OUTPUT_DELAY_NS( 6.0, 2.0 ), // Somewhat arbitrary

    CHIP_DESC_END
};
#endif

