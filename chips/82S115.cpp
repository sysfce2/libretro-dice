#include "../chip_desc.h"
#include "rom.h"

/*
82S115
4096-Bit Bipolar PROM (512x8)

    +------+
 A3 |1   24| Vcc
 A4 |2   23| A2
 A5 |3   22| A1
 A6 |4   21| A0
 A7 |5   20| CE1/
 A8 |6   19| CE2
 O1 |7   18| STROBE
 O2 |8   17| O8
 O3 |9   16| O7
 O4 |10  15| O6
 FE2|11  14| O5
 GND|12  13| FE1
    +------+
*/

// TODO: Chip selects, strobe, etc.

template <int BIT>
CHIP_LOGIC( 82S115 )
{
    const RomDesc* rom = (const RomDesc*)custom_data;

    int addr = (pin[6] << 8) | (pin[5] << 7) | (pin[4] << 6) | (pin[3] << 5) | 
               (pin[2] << 4) | (pin[1] << 3) | (pin[23] << 2) | (pin[22] << 1) | pin[21];

    int P = (BIT < 4) ? (BIT + 7) : (BIT + 10);

    pin[P] = (RomDesc::get_data(rom, addr) >> BIT) & 1;
}

CHIP_DESC( 82S115 ) = 
{
    CHIP_START( 82S115<0> )
        INPUT_PINS( 21, 22, 23, 1, 2, 3, 4, 5, 6 )
        OUTPUT_PIN( 7 )
        OUTPUT_DELAY_NS( 35.0, 35.0 ),

    CHIP_START( 82S115<1> )
        INPUT_PINS( 21, 22, 23, 1, 2, 3, 4, 5, 6 )
        OUTPUT_PIN( 8 )
        OUTPUT_DELAY_NS( 35.0, 35.0 ),

    CHIP_START( 82S115<2> )
        INPUT_PINS( 21, 22, 23, 1, 2, 3, 4, 5, 6 )
        OUTPUT_PIN( 9 )
        OUTPUT_DELAY_NS( 35.0, 35.0 ),

    CHIP_START( 82S115<3> )
        INPUT_PINS( 21, 22, 23, 1, 2, 3, 4, 5, 6 )
        OUTPUT_PIN( 10 )
        OUTPUT_DELAY_NS( 35.0, 35.0 ),

    CHIP_START( 82S115<4> )
        INPUT_PINS( 21, 22, 23, 1, 2, 3, 4, 5, 6 )
        OUTPUT_PIN( 14 )
        OUTPUT_DELAY_NS( 35.0, 35.0 ),

    CHIP_START( 82S115<5> )
        INPUT_PINS( 21, 22, 23, 1, 2, 3, 4, 5, 6 )
        OUTPUT_PIN( 15 )
        OUTPUT_DELAY_NS( 35.0, 35.0 ),

    CHIP_START( 82S115<6> )
        INPUT_PINS( 21, 22, 23, 1, 2, 3, 4, 5, 6 )
        OUTPUT_PIN( 16 )
        OUTPUT_DELAY_NS( 35.0, 35.0 ),

    CHIP_START( 82S115<7> )
        INPUT_PINS( 21, 22, 23, 1, 2, 3, 4, 5, 6 )
        OUTPUT_PIN( 17 )
        OUTPUT_DELAY_NS( 35.0, 35.0 ),

   	CHIP_DESC_END
};

