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
    { "Pong",           circuit_pong,           "pong",             "Atari", 1972 },
    { "Pong Doubles",   circuit_pongdoubles,    "pongdoubles",      "Atari", 1973 },
    { "Rebound",        circuit_rebound,        "rebound",          "Atari", 1974 },
    { "Gotcha",         circuit_gotcha,         "gotcha",           "Atari", 1973 },
    { "Space Race",     circuit_spacerace,      "spacerace",        "Atari", 1973 },
    { "Stunt Cycle",    circuit_stuntcycle,     "stuntcycle",       "Atari", 1976 },
    { "TV Basketball",  circuit_tvbasketball,   "tvbasketball",     "Midway", 1974 },
    { "Breakout",       circuit_breakout,       "breakout",         "Atari", 1976 },
    { "Anti-Aircraft",  circuit_antiaircraft,   "antiaircraft",     "Atari", 1975 },
    { "Attack",         circuit_attack,         "attack",           "Exidy", 1977 }
};

static const int game_list_size = sizeof(game_list) / sizeof(GameDesc);
