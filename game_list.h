#include "circuit_desc.h"

extern CIRCUIT_LAYOUT( pong );
extern CIRCUIT_LAYOUT( rebound );
extern CIRCUIT_LAYOUT( gotcha );
extern CIRCUIT_LAYOUT( spacerace );
extern CIRCUIT_LAYOUT( stuntcycle );
extern CIRCUIT_LAYOUT( pongdoubles );
extern CIRCUIT_LAYOUT( tvbasketball );
extern CIRCUIT_LAYOUT( breakout );


struct GameDesc
{
    const char* name;
    CircuitDesc* desc;
    const char* command_line;
};

static const GameDesc game_list[] =
{
    { "Pong", circuit_pong, "pong" },
    { "Pong Doubles", circuit_pongdoubles, "pongdoubles" },
    { "Rebound", circuit_rebound, "rebound" },
    { "Gotcha", circuit_gotcha, "gotcha" },
    { "Space Race", circuit_spacerace, "spacerace" },
    { "Stunt Cycle", circuit_stuntcycle, "stuntcycle" },
    { "TV Basketball", circuit_tvbasketball, "tvbasketball" },
    { "Breakout", circuit_breakout, "breakout" }
};
