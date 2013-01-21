#ifndef CIRCUITDESC_H
#define CIRCUITDESC_H

#include "chip_desc.h"
#include "chip_list.h"
#include "video_desc.h"

enum CircuitDescType
{
    CHIP_INST = 0,
    CONNECTION,
    VIDEO_INST,
    CIRCUIT_END
};

struct ChipInstance
{
    const char* name;
    ChipDesc* chip;
	void* custom_data;
};

struct ConnectionDesc
{
    const char* name1;
    const char* name2;
    uint8_t pin1;
	uint8_t pin2;
};

struct CircuitDesc
{
    CircuitDescType type;
	union U
    {
		ChipInstance instance;
    	ConnectionDesc connection;
        VideoDesc* video;

        constexpr U(const char* n, ChipDesc* c, void* d) : instance({n, c, d}) { }
        constexpr U(const char* n1, uint8_t p1, const char* n2, uint8_t p2) : connection({n1, n2, p1, p2}) { }
        constexpr U(VideoDesc* v) : video(v) { }
	} u;

    constexpr CircuitDesc(const char* n, ChipDesc* c, void* d = NULL) : type(CHIP_INST), u(n, c, d) { }
	constexpr CircuitDesc(const char* n1, uint8_t p1, const char* n2, uint8_t p2) : type(CONNECTION), u(n1, p1, n2, p2) { }
    constexpr CircuitDesc(VideoDesc* v) : type(VIDEO_INST), u(v) { }
    constexpr CircuitDesc() : type(CIRCUIT_END), u(NULL) { }
};

#define VCC "_VCC", 0
#define GND "_GND", 0

#define CIRCUIT_LAYOUT( name ) CircuitDesc circuit_##name[]

#define CHIP( name, desc... ) CircuitDesc(name, chip_##desc)
#define CONNECTION( args... ) CircuitDesc(args)
#define VIDEO( name )         CircuitDesc(&video_##name)
#define CIRCUIT_LAYOUT_END 	  CircuitDesc()

#define OHM( val ) double(val)
#define K_OHM( val ) (double(val)*1e3)
#define M_OHM( val ) (double(val)*1e6)

#define M_FARAD( val ) (double(val)*1e-3)
#define U_FARAD( val ) (double(val)*1e-6)
#define N_FARAD( val ) (double(val)*1e-9)
#define P_FARAD( val ) (double(val)*1e-12)

#define DELAY_MS( t ) (double(t)*1e-3)
#define DELAY_US( t ) (double(t)*1e-6)
#define DELAY_NS( t ) (double(t)*1e-9)
#define DELAY_PS( t ) (double(t)*1e-12)

#endif
