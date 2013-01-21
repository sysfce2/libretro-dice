#ifndef CHIP_H
#define CHIP_H

#include <cstddef>
#include <stdint.h>
#include <vector>

#define INITIAL_STATE_LENGTH 4

class Circuit;
class ChipDesc;
class Chip;

enum ChipType { SIMPLE_CHIP = 0, BASIC_CHIP, CUSTOM_CHIP };

enum ChipState { ACTIVE = 0, PASSIVE, ASLEEP };

struct ChipLink
{
    Chip* chip;
    uint32_t mask;

    ChipLink() : chip(NULL), mask(0) {}
    ChipLink(Chip* c, uint32_t m) : chip(c), mask(m) {}
};

typedef typename std::vector<ChipLink>::iterator LinkIterator;

class Chip
{
public:
    Circuit* circuit;
	
	int inputs;
    int output;

	int prev_input_mask;
	int prev_output_mask;

	uint64_t delay[2];

	uint64_t pending_event;

    int active_outputs;
    int active_inputs;
    std::vector<ChipLink> output_links;
    std::vector<ChipLink> input_links;

    uint32_t event_count;
	ChipType type;
	
	union {
		uint64_t lut_data;
		uint32_t* lut;
		void (*custom_update)(Chip* chip, int mask);
	};

	void* custom_data;

    // Begin new stuff
    ChipState state;
    
	std::vector<uint64_t> output_events;
	int first_output_event;
    int current_output_event;
    
    std::vector<uint64_t> last_input_event_time;
   	int initial_inputs[INITIAL_STATE_LENGTH];
    uint64_t initial_time[INITIAL_STATE_LENGTH];
    uint64_t cycle_time;
    uint64_t sleep_time;
    uint64_t activation_time;
    uint64_t offset_time;

    bool visited;
    // End new stuff
	
	Chip(Circuit* cir, ChipDesc* desc, void* custom = NULL);
    void connect(Chip* chip, ChipDesc* desc, uint8_t pin);
	void initialize();

    void update_inputs(uint32_t mask);
	void update_output();

    //virtual void custom_update(uint32_t mask) = 0;

    bool activation_check(uint64_t min_time, uint64_t max_time);
    void activate_inputs();
    //void activate_outputs();
    void deactivate_outputs();
    void wake_up();

    bool dynamic; // dynamic = driven by inputs, static = driven only by clock
    int MAX_INPUT_EVENTS;
    uint64_t MAX_CYCLE_TIME;
    void make_dynamic();

    static int next_bit(uint32_t x);

    //For debugging
    uint64_t total_event_count;
    uint64_t activation_count;
};

#endif
