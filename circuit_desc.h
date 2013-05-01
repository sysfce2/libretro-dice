#ifndef CIRCUITDESC_H
#define CIRCUITDESC_H

#include "chip_desc.h"
#include "chip_list.h"
#include "video_desc.h"
#include "audio_desc.h"

enum CircuitDescType
{
    CHIP_INST = 0,
    CONNECTION,
    NET,
    VIDEO_INST,
    AUDIO_INST,
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

struct NetDesc
{
    const char* name;
    const char* chip;
    uint8_t pin;
};

struct CircuitDesc
{
    CircuitDescType type;
	union U
    {
		ChipInstance instance;
    	ConnectionDesc connection;
        NetDesc net;
        VideoDesc* video;
        AudioDesc* audio;

        constexpr U(const char* n, ChipDesc* c, void* d) : instance({n, c, d}) { }
        constexpr U(const char* n1, uint8_t p1, const char* n2, uint8_t p2) : connection({n1, n2, p1, p2}) { }
        constexpr U(const char* n, const char* c, uint8_t p) : net({n, c, p}) { }
        constexpr U(VideoDesc* v) : video(v) { }
        constexpr U(AudioDesc* a) : audio(a) { }
	} u;

    constexpr CircuitDesc(const char* n, ChipDesc* c, void* d = NULL) : type(CHIP_INST), u(n, c, d) { }
	constexpr CircuitDesc(const char* n1, uint8_t p1, const char* n2, uint8_t p2) : type(CONNECTION), u(n1, p1, n2, p2) { }
    constexpr CircuitDesc(const char* n, const char* c, uint8_t p) : type(NET), u(n, c, p) { }
    constexpr CircuitDesc(VideoDesc* v) : type(VIDEO_INST), u(v) { }
    constexpr CircuitDesc(AudioDesc* a) : type(AUDIO_INST), u(a) { }
    constexpr CircuitDesc() : type(CIRCUIT_END), u(NULL, 0, NULL, 0) { }
};

#define VCC "_VCC", 1
#define GND "_GND", 1

#define CIRCUIT_LAYOUT( name ) CircuitDesc circuit_##name[]

#define CHIP( name, desc... ) CircuitDesc(name, chip_##desc)
#define CONNECTION( args... ) CircuitDesc(args)
//#define NET( args... )        CircuitDesc(args)
#define VIDEO( name )         CircuitDesc(&video_##name)
#define AUDIO( name )         CircuitDesc(&audio_##name)
#define CIRCUIT_LAYOUT_END 	  CircuitDesc()

#define DISABLE_OPTIMIZATION(chip, pin) CircuitDesc("_DEOPTIMIZER", 1, chip, pin)

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
