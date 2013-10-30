#ifndef DIPSWITCH_H
#define DIPSWITCH_H

#include "../chip_desc.h"
#include "555astable.h"
#include "555mono.h"

/* Standard 1-bit DIP Switch. Can be SPDT or DPDT. */
class DipswitchDesc
{
public:
    
    unsigned state;

    const char* name;
    const char* desc;
    const char* settings[2];

    DipswitchDesc(const char* n, const char* d, int default_state,
                  const char* setting1, const char* setting2) 
        : name(n), desc(d), state(default_state), settings{setting1, setting2} { }
    
    static CUSTOM_LOGIC( logic );
};

extern CHIP_DESC( DIPSWITCH );



/* SP4T Switch. Assumes outputs are tied high when not selected by switch. */
class DipswitchSP4TDesc
{
public:
    
    unsigned state;

    const char* name;
    const char* desc;
    const char* settings[4];

    DipswitchSP4TDesc(const char* n, const char* d, int default_state,
                      const char* setting1, const char* setting2,
                      const char* setting3, const char* setting4) 
        : name(n), desc(d), state(default_state), 
          settings{setting1, setting2, setting3, setting4} { }
    
    template<int T> static CUSTOM_LOGIC( logic );
};

extern CHIP_DESC( DIPSWITCH_SP4T );




/* AMP 53137. 4-bit Rotary Hex DIP Switch. */
class Dipswitch53137Desc
{
public:
    
    unsigned state;

    const char* name;
    const char* desc;
    const char* settings[16];

    Dipswitch53137Desc(const char* n, const char* d, int default_state,
                       const char* setting1, const char* setting2,
			           const char* setting3 = NULL, const char* setting4 = NULL,
			           const char* setting5 = NULL, const char* setting6 = NULL,
			           const char* setting7 = NULL, const char* setting8 = NULL,
			           const char* setting9 = NULL, const char* setting10 = NULL,
			           const char* setting11 = NULL, const char* setting12 = NULL,
			           const char* setting13 = NULL, const char* setting14 = NULL,
			           const char* setting15 = NULL, const char* setting16 = NULL)
        : name(n), desc(d), state(default_state), 
          settings{setting1, setting2, setting3, setting4,
                   setting5, setting6, setting7, setting8,
                   setting9, setting10, setting11, setting12,
                   setting13, setting14, setting15, setting16} { }

    template<int BIT> static CUSTOM_LOGIC( logic );
};

extern CHIP_DESC( 53137 );



/* 4SPST 4-bit Hex DIP Switch. */
typedef Dipswitch53137Desc Dipswitch4SP4TDesc;

extern CHIP_DESC( DIPSWITCH_4SPST );





/* Potentiometer */
template <typename T, double (T::*r)>
class PotentiometerDesc
{
public:
    double min_val, max_val, current_val;
    T& output;

    const char* name;
    const char* desc;

    PotentiometerDesc(const char* n, const char* d, double default_val, double min, double max, T& t) 
        : name(n), desc(d), current_val(default_val), min_val(min), max_val(max), output(t) { }

    static CUSTOM_LOGIC( logic );
};

typedef PotentiometerDesc<Astable555Desc, &Astable555Desc::r1> PotentimeterAstable555Desc; 
typedef PotentiometerDesc<Mono555Desc, &Mono555Desc::r> PotentimeterMono555Desc; 

extern CHIP_DESC( POT_555_ASTABLE );
extern CHIP_DESC( POT_555_MONO );

#define POTENTIOMETER_CONNECTION( c1, c2 ) CircuitDesc(c1, i3, c2, i3)

#endif
