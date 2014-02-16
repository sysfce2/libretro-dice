#include "circuit_desc.h"

extern CIRCUIT_LAYOUT( pong );
extern CIRCUIT_LAYOUT( rebound );
extern CIRCUIT_LAYOUT( gotcha );
extern CIRCUIT_LAYOUT( spacerace );
extern CIRCUIT_LAYOUT( stuntcycle );
extern CIRCUIT_LAYOUT( pongdoubles );
extern CIRCUIT_LAYOUT( tvbasketball );
extern CIRCUIT_LAYOUT( breakout );
extern CIRCUIT_LAYOUT( antiaircraft );
extern CIRCUIT_LAYOUT( attack );
extern CIRCUIT_LAYOUT( sharkjaws );
extern CIRCUIT_LAYOUT( quadrapong );
extern CIRCUIT_LAYOUT( jetfighter );
extern CIRCUIT_LAYOUT( crashnscore );
extern CIRCUIT_LAYOUT( crossfire );
extern CIRCUIT_LAYOUT( pinpong );
extern CIRCUIT_LAYOUT( cleansweep );
extern CIRCUIT_LAYOUT( wipeout );


struct GameDesc
{
    const char* name;
    CircuitDesc* desc;
    const char* command_line;
    
    const char* manufacturer;
    unsigned year;

    bool operator<(const GameDesc& g) const { return strcmp(name, g.name) < 0; }
};

static GameDesc game_list[] =
{
    { "Pong",           circuit_pong,           "pong",             "Atari",     1972 },
    { "Pong Doubles",   circuit_pongdoubles,    "pongdoubles",      "Atari",     1973 },
    { "Rebound",        circuit_rebound,        "rebound",          "Atari",     1974 },
    { "Gotcha",         circuit_gotcha,         "gotcha",           "Atari",     1973 },
    { "Space Race",     circuit_spacerace,      "spacerace",        "Atari",     1973 },
    { "Stunt Cycle",    circuit_stuntcycle,     "stuntcycle",       "Atari",     1976 },
    { "TV Basketball",  circuit_tvbasketball,   "tvbasketball",     "Midway",    1974 },
    { "Breakout",       circuit_breakout,       "breakout",         "Atari",     1976 },
    { "Anti-Aircraft",  circuit_antiaircraft,   "antiaircraft",     "Atari",     1975 },
    { "Attack",         circuit_attack,         "attack",           "Exidy",     1977 },
    { "Shark Jaws",     circuit_sharkjaws,      "sharkjaws",        "Atari",     1975 },
    { "Quadrapong",     circuit_quadrapong,     "quadrapong",       "Atari",     1974 },
    { "Jet Fighter",    circuit_jetfighter,     "jetfighter",       "Atari",     1975 },
    { "Crash 'N Score", circuit_crashnscore,    "crashnscore",      "Atari",     1975 },
    { "Crossfire",      circuit_crossfire,      "crossfire",        "Atari",     1975 },
    { "Pin Pong",       circuit_pinpong,        "pinpong",          "Atari",     1974 },
    { "Clean Sweep",    circuit_cleansweep,     "cleansweep",       "Ramtek",    1974 },
    { "Wipe Out",       circuit_wipeout,        "wipeout",          "Ramtek",    1974 }
};

static const int game_list_size = sizeof(game_list) / sizeof(GameDesc);
