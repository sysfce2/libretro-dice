#ifndef CHIPDESC_H
#define CHIPDESC_H

#include <stdint.h>
#include "chip.h"

#define MAX_PINS			32
#define MAX_EXTERNAL_PINS 	24
#define MAX_INTERNAL_PINS 	8
#define MAX_INPUT_PINS      23

enum InternalPin
{
	i1 = 25, i2 = 26, i3 = 27, i4 = 28, i5 = 29, i6 = 30, i7 = 31, i8 = 32
};

typedef void (*ChipLogic)(int* pin, int* prev_pin, void* custom_data);
typedef void (*CustomLogic)(Chip* chip, int mask);

struct ChipDesc
{
    ChipLogic logic_func;
	CustomLogic custom_logic;
    
    double output_delay[2];

    uint8_t input_pins[MAX_INPUT_PINS+1];
    uint8_t output_pin;
    uint8_t prev_input_pin;
    uint8_t prev_output_pin;

    ChipDesc(ChipLogic cl) : logic_func(cl), custom_logic(NULL), 
                             output_pin(0), prev_input_pin(0), prev_output_pin(0)
    {
        set_input_pins(0);
        set_output_delay(0.0, 0.0);
    }
    ChipDesc(CustomLogic cl) : logic_func(NULL), custom_logic(cl), 
                               output_pin(0), prev_input_pin(0), prev_output_pin(0)
    {
        set_input_pins(0);
        set_output_delay(0.0, 0.0);
    }

    ChipDesc& set_input_pins(uint8_t p1,      uint8_t p2  = 0, uint8_t p3  = 0, uint8_t p4 = 0,
                             uint8_t p5  = 0, uint8_t p6  = 0, uint8_t p7  = 0, uint8_t p8 = 0,
                             uint8_t p9  = 0, uint8_t p10 = 0, uint8_t p11 = 0, uint8_t p12 = 0,
                             uint8_t p13 = 0, uint8_t p14 = 0, uint8_t p15 = 0, uint8_t p16 = 0,
                             uint8_t p17 = 0, uint8_t p18 = 0, uint8_t p19 = 0, uint8_t p20 = 0,
                             uint8_t p21 = 0, uint8_t p22 = 0, uint8_t p23 = 0)
    {
        input_pins[0] = p1;   input_pins[1] = p2;   input_pins[2] = p3;   input_pins[3] = p4;
        input_pins[4] = p5;   input_pins[5] = p6;   input_pins[6] = p7;   input_pins[7] = p8;
        input_pins[8] = p9;   input_pins[9] = p10;  input_pins[10] = p11; input_pins[11] = p12;
        input_pins[12] = p13; input_pins[13] = p14; input_pins[14] = p15; input_pins[15] = p16;
        input_pins[16] = p17; input_pins[17] = p18; input_pins[18] = p19; input_pins[19] = p20;
        input_pins[20] = p21; input_pins[21] = p22; input_pins[22] = p23; input_pins[23] = 0;
        return *this;
    }
    ChipDesc& set_output_pin(uint8_t p)
    {
        output_pin = p;
        return *this;
    }
    ChipDesc& set_prev_input_pin(uint8_t p)
    {
        prev_input_pin = p;
        return *this;
    }
    ChipDesc& set_prev_output_pin(uint8_t p)
    {
        prev_output_pin = p;
        return *this;
    }
    ChipDesc& set_output_delay(double tp_lh, double tp_hl)
    {
        output_delay[0] = tp_lh;
        output_delay[1] = tp_hl;
        return *this;
    }
    bool endOfDesc()
    {
        return logic_func == NULL && custom_logic == NULL;
    }
};


#define CHIP_DESC( name ) ChipDesc chip_##name[]

#define CHIP_START( name ) 				ChipDesc(ChipLogic(&logic_##name))
#define CUSTOM_CHIP_START( name )	    ChipDesc(CustomLogic(name))
#define INPUT_PINS( pins... ) 			.set_input_pins(pins)
#define OUTPUT_PIN( pin ) 				.set_output_pin(pin)
#define PREV_INPUT_PIN( pin ) 			.set_prev_input_pin(pin)
#define PREV_OUTPUT_PIN( pin ) 			.set_prev_output_pin(pin)
#define OUTPUT_DELAY_S( tp_lh, tp_hl )  .set_output_delay(double(tp_lh), double(tp_hl))
#define OUTPUT_DELAY_MS( tp_lh, tp_hl ) .set_output_delay(double(tp_lh)*1e-3, double(tp_hl)*1e-3)
#define OUTPUT_DELAY_US( tp_lh, tp_hl ) .set_output_delay(double(tp_lh)*1e-6, double(tp_hl)*1e-6)
#define OUTPUT_DELAY_NS( tp_lh, tp_hl ) .set_output_delay(double(tp_lh)*1e-9, double(tp_hl)*1e-9)
#define OUTPUT_DELAY_PS( tp_lh, tp_hl ) .set_output_delay(double(tp_lh)*1e-12, double(tp_hl)*1e-12)
#define CHIP_DESC_END                   ChipDesc(ChipLogic(NULL))

#define CHIP_LOGIC( name ) void logic_##name(int* pin, int* prev_pin, void* custom_data)
#define CUSTOM_LOGIC( name ) void name(Chip* chip, int mask)

#define CHIP_ALIAS( name, desc ) constexpr ChipDesc* chip_##name = chip_##desc

#endif
