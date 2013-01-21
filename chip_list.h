#include "chip_desc.h"

#include "chips/audio.h"
#include "chips/input.h"
#include "chips/video.h"
#include "chips/dipswitch.h"
#include "chips/vcd_log.h"

#include "chips/555astable.h"
#include "chips/555mono.h"
#include "chips/555pwm.h"
#include "chips/9602.h"

#include "chips/capacitor.h"
#include "chips/diode_matrix.h"
#include "chips/rom.h"

extern CHIP_DESC( CLOCK_14_318_MHZ );
extern CHIP_DESC( CLOCK_6_MHZ );

extern CHIP_DESC( 7400 );
extern CHIP_DESC( 7402 );
extern CHIP_DESC( 7404 );
extern CHIP_DESC( 7408 );
extern CHIP_DESC( 7410 );
extern CHIP_DESC( 7411 );
extern CHIP_DESC( 7420 );
extern CHIP_DESC( 7425 );
extern CHIP_DESC( 7427 );
extern CHIP_DESC( 7430 );
extern CHIP_DESC( 7432 );
extern CHIP_DESC( 7437 );
extern CHIP_DESC( 7448 );
extern CHIP_DESC( 7450 );
extern CHIP_DESC( 7451 );
extern CHIP_DESC( 7454 );
extern CHIP_DESC( 7474 );
extern CHIP_DESC( 7476 );
extern CHIP_DESC( 7483 );
extern CHIP_DESC( 7485 );
extern CHIP_DESC( 7486 );
extern CHIP_DESC( 7490 );
extern CHIP_DESC( 7493 );
extern CHIP_DESC( 7493A );
extern CHIP_DESC( 74107 );
extern CHIP_DESC( 74151 );
extern CHIP_DESC( 74153 );
extern CHIP_DESC( 74155 );
extern CHIP_DESC( 74165 );
extern CHIP_DESC( 74175 );
extern CHIP_DESC( 74192 );
extern CHIP_DESC( 74193 );
extern CHIP_DESC( 74194 );
extern CHIP_DESC( 74279 );
extern CHIP_DESC( 9310 );
extern CHIP_DESC( 9311 );
extern CHIP_DESC( 9312 );
extern CHIP_DESC( 9316 );
extern CHIP_DESC( 9322 );

extern CHIP_DESC( 82S16 );
extern CHIP_DESC( 82S115 );
extern CHIP_DESC( 82S123 );

extern CHIP_DESC( LATCH );

extern CHIP_DESC( CLK_GATE );

static CHIP_ALIAS( 74154, 9311 );
static CHIP_ALIAS( 74161, 9316 );
static CHIP_ALIAS( 74157, 9322 );
