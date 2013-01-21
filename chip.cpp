#include <cstdio>
#include <cstdlib>

#include "chip.h"
#include "chip_desc.h"
#include "circuit.h"

#define DEBUG
#undef DEBUG

#ifdef DEBUG
#define debug_printf(args...) printf(args)
//#define debug_printf(args...) do { if(this == circuit->chips[3]/*->input_links[0].chip*/) printf(args); }while(0)
//#define debug_printf(args...)
#else
#define debug_printf(args...)
//#define debug_printf(args...) printf(args)
#endif


void Chip::initialize()
{
	int new_out = output;

    //active_inputs = (1 << input_links.size()) - 1;

    // Set previous input state - no clock edges during init
    if(inputs & prev_input_mask & ((1 << input_links.size()) - 1))
        inputs |= prev_input_mask;
    else
	    inputs &= ~prev_input_mask;

	switch(type) {
		case SIMPLE_CHIP: new_out = (lut_data >> inputs) & 1; break;
		case BASIC_CHIP:  new_out = (lut[inputs >> 5] >> (inputs & 0x1f)) & 1; break;
        case CUSTOM_CHIP: custom_update(this, 0); return;
	}

    // Set previous input state
    /*if(inputs & prev_input_mask & ((1 << input_links.size()) - 1))
        inputs |= prev_input_mask;
    else
	    inputs &= ~prev_input_mask;*/

	if(new_out != output)
	{
		output = new_out;
        
        if(output)
            inputs |= prev_output_mask;
        else
            inputs &= ~prev_output_mask;

		/*for(LinkIterator it = output_links.begin(); it != output_links.end(); ++it)
		{
            if(it->chip == this) continue;
            
            it->chip->inputs ^= it->mask;
			it->chip->initialize();
		}*/

        for(LinkIterator it = output_links.begin(); it != output_links.end(); ++it)
            it->chip->inputs ^= it->mask;

        for(LinkIterator it = output_links.begin(); it != output_links.end(); ++it)
        {
            if(it->chip == this) continue;
            it->chip->initialize();
        }
	}
}

Chip::Chip(Circuit* cir, ChipDesc* desc, void* custom) : 
	circuit(cir), custom_data(custom), inputs(0), output(0), prev_input_mask(0), prev_output_mask(0), 
    pending_event(0), active_outputs(0), active_inputs(0), state(ACTIVE), current_output_event(0),
    cycle_time(0), event_count(0), activation_count(0), visited(false), activation_time(0),
    type(CUSTOM_CHIP), first_output_event(0), total_event_count(0), 
    //dynamic(false), MAX_INPUT_EVENTS(524288), MAX_CYCLE_TIME(50.0e-3 / 1.0e-12)
    dynamic(true), MAX_INPUT_EVENTS(4096), MAX_CYCLE_TIME(1.0e-3 / 1.0e-12)
{
    if(desc->logic_func == NULL && desc->custom_logic == NULL)
        return;

    int num_input_pins;
    // Count number of input pins
    for(num_input_pins = 0; desc->input_pins[num_input_pins] != 0; num_input_pins++);

    delay[0] = uint64_t(desc->output_delay[0] / Circuit::timescale);
	delay[1] = uint64_t(desc->output_delay[1] / Circuit::timescale);

    for(int i = 0; i < INITIAL_STATE_LENGTH; i++)
    {
        initial_time[i] = 0;
        initial_inputs[i] = 0;
    }

    input_links.resize(num_input_pins);
    last_input_event_time.resize(num_input_pins);

	// Don't set up LUT of custom chips
	if(desc->custom_logic)
	{
		type = CUSTOM_CHIP;
        custom_update = desc->custom_logic;
        return;
	}

    int lut_size = num_input_pins;

	if(desc->prev_input_pin)
	{
		lut_size++;
		prev_input_mask = (1 << num_input_pins);
		for(int i = 0; i < num_input_pins; i++)
			if(desc->input_pins[i] == desc->prev_input_pin)
				prev_input_mask |= (1 << i);
	}
	if(desc->prev_output_pin)
	{
		prev_output_mask = (1 << lut_size);
        lut_size++;
	}

	if(lut_size <= 6)
	{
		type = SIMPLE_CHIP;
		lut_data = 0;
        //type = BASIC_CHIP;
		//lut = new uint32_t[2];
        //lut[0] = lut[1] = 0;
	}
	else
	{
		type = BASIC_CHIP;
		lut = new uint32_t[1 << (lut_size-5)];
		memset(lut, 0, sizeof(uint32_t)*(1 << (lut_size-5)));
	}

	// Fill the LUT
	for(int i = 0; i < (1 << lut_size); i++)
	{
		int pin[MAX_PINS+1] = { 0 };
		int prev_pin[MAX_PINS+1] = { 0 };
		int x = i;

		for(int j = 0; j < num_input_pins; j++)
		{
			pin[desc->input_pins[j]] = x & 1;
			x >>= 1;
		}
		
		if(desc->prev_input_pin)
		{
			prev_pin[desc->prev_input_pin] = x & 1;
			x >>= 1;
		}

		if(desc->prev_output_pin)
			prev_pin[desc->prev_output_pin] = x & 1;

		desc->logic_func(pin, prev_pin, custom_data);
		
		if(type == SIMPLE_CHIP)
			lut_data |= ((pin[desc->output_pin] ? 1ull : 0) << i);
		else
            lut[i >> 5] |= ((pin[desc->output_pin] ? 1 : 0) << (i & 0x1f));
	}
}

void Chip::connect(Chip* chip, ChipDesc* desc, uint8_t pin)
{
    //this is output, chip is input

    for(int i = 0; desc->input_pins[i]; i++)
        if(desc->input_pins[i] == pin)
        {
			// If output links already contains a link to this chip,
            // OR with its mask, otherwise add new link
            int x;
            for(x = 0; x < output_links.size(); x++)
                if(output_links[x].chip == chip) break;

            if(x != output_links.size())
            {
                output_links[x].mask |= (1 << i);
            }
            else
            {
                output_links.push_back(ChipLink(chip, 1 << i));
                active_outputs |= (1 << x);
            }
            chip->input_links[i] = ChipLink(this, 1 << x);

            return;
        }
}


void Chip::wake_up()
{
    debug_printf("waking up %x\n", this);

    state = ACTIVE;

    // Get chip up to date
    if(cycle_time == 0) return;

    uint64_t time = (circuit->global_time - sleep_time) % cycle_time;
                    
    debug_printf("sleeptime: %lld global t: %lld cyc: %lld t: %lld\n",
                 sleep_time, circuit->global_time, cycle_time, time);

    while(time > output_events[current_output_event])
    {
        time -= output_events[current_output_event++];

        output ^= 1;
        //inputs ^= prev_output_mask;

        if(current_output_event == output_events.size())
            current_output_event = first_output_event;
    }

    pending_event = circuit->queue_push(this, output_events[current_output_event++] - time);
    
    if(current_output_event == output_events.size())
        current_output_event = first_output_event;
}

#if 1
void Chip::update_inputs(uint32_t mask)
{
    //total_event_count++;
    
    event_count++;

    const uint64_t global_time = circuit->global_time;

    if(type == CUSTOM_CHIP)
	{
		custom_update(this, mask);
		return;
	}

    //debug_printf("UPDATE INPUTS: chip: %x mask: %d inp: %d t: %lld cyc:%lld\n", this, mask, inputs, circuit->global_time, cycle_time);

    if(state == ASLEEP)
    {
        wake_up();
    }

    //for(int i = 0; mask >> i; i++)
    {
    //    i += Chip::next_bit(mask >> i);
    //    last_input_event_time[i] = global_time;
    }
    if(mask)
        last_input_event_time[Chip::next_bit(mask)] = global_time;

    Chip* input_chip = input_links[next_bit(mask)].chip;

    if(state == ACTIVE || event_count >= MAX_INPUT_EVENTS || (global_time-initial_time[0]) > MAX_CYCLE_TIME)
    {
        deactivate_outputs();
    }
    else if(mask && event_count >= INITIAL_STATE_LENGTH) // TODO: mask nonzero check needed?
    {
        if(input_chip->state == ACTIVE && input_chip->activation_time > initial_time[INITIAL_STATE_LENGTH-1])
            deactivate_outputs();
    }
        
    inputs ^= mask;

    active_inputs |= mask;

    int new_out;

	switch(type) {
		case SIMPLE_CHIP: new_out = (lut_data >> inputs) & 1; break;
	    default:          new_out = (lut[inputs >> 5] >> (inputs & 0x1f)) & 1; break; 
	}

    for(int i = event_count; i < INITIAL_STATE_LENGTH; i++)
    {
        initial_inputs[i] = inputs;
        initial_time[i] = global_time;
    }

    if(activation_check(initial_time[0], initial_time[INITIAL_STATE_LENGTH-1]))
    {
        activate_inputs();
        return;
    }

    if(new_out != output && pending_event == 0)
    {
        pending_event = circuit->queue_push(this, delay[output]);
        inputs ^= prev_output_mask;

        output_events.push_back(global_time - cycle_time - initial_time[0] + delay[output]);
        cycle_time += output_events.back();

        //debug_printf("new_event: t:%lld %x %lld\n", global_time, this, output_events.back());
    }
    else if(pending_event && new_out == output)
    {
        debug_printf("remove_event: t:%lld %x %lld\n", global_time,  this, output_events.back());
        
        pending_event = 0;
        inputs ^= prev_output_mask;
        //debug_printf("ERROR UNHANDLED STATE 2\n"); fflush(stdout);
        if(!output_events.empty())
        {
            cycle_time -= output_events.back();
            output_events.pop_back();
        }

        // Perform activation check again, since inputs was changed
        for(int i = event_count; i < INITIAL_STATE_LENGTH; i++)
        {
            initial_inputs[i] = inputs;
            initial_time[i] = global_time;
        }
    
        if(activation_check(initial_time[0], initial_time[INITIAL_STATE_LENGTH-1]))
        {
            activate_inputs();
            return;
        }
    }

    /*for(int i = event_count; i < INITIAL_STATE_LENGTH; i++)
    {
        initial_inputs[i] = inputs;
        initial_time[i] = global_time;
    }

    if(activation_check(initial_time[0], initial_time[INITIAL_STATE_LENGTH-1]))
    {
        activate_inputs();
        return;
    }*/

    if(mask & prev_input_mask) inputs ^= (prev_input_mask & ~mask);
}

#else

void Chip::update_inputs(uint32_t mask)
{
    event_count++;

    if(type == CUSTOM_CHIP)
	{
		custom_update(this, mask);
		return;
	}

    //debug_printf("UPDATE INPUTS: chip: %x mask: %d inp: %d t: %lld cyc:%lld\n", this, mask, inputs, circuit->global_time, cycle_time);

	inputs ^= mask;

	int new_out;
	
    if(state == ACTIVE)
        state = PASSIVE;

	switch(type) {
		case SIMPLE_CHIP: new_out = (lut_data >> inputs) & 1; break;
		case BASIC_CHIP:  new_out = (lut[inputs >> 5] >> (inputs & 0x1f)) & 1; break; 
	}

    if(new_out != output && pending_event == 0)
    {
        pending_event = circuit->queue_push(this, delay[output]);
        inputs ^= prev_output_mask;

        debug_printf("new_event: %x %lld\n", this, output_events.back());
    }
    else if(pending_event && new_out == output)
	{
		debug_printf("remove_event: %x %lld\n", this, output_events.back());
        
        pending_event = 0;
        inputs ^= prev_output_mask;
    }

    if(mask & prev_input_mask) inputs ^= (prev_input_mask & ~mask);
}

#endif

inline int Chip::next_bit(uint32_t x)
{
#if 0
    // Computes number of trailing zeros
    static const uint8_t de_bruijn_bit_position[32] = 
    {
        0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
        31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
    };

    return de_bruijn_bit_position[((uint32_t)((x & -x) * 0x077CB531U)) >> 27];
#else
    return __builtin_ctz(x);
#endif  
}

void Chip::update_output()
{
    //debug_printf("update output: %p\n", this);    

    // Don't just iterate through this the normal way -
    // active_outputs can be modified through update_inputs().
    for(int mask = ~0; active_outputs & mask;)
    {
        int i = Chip::next_bit(active_outputs & mask);
        ChipLink* link = &output_links[i];
        link->chip->update_inputs(link->mask);
        mask &= ~(1 << i);
    }

	output ^= 1;
	//inputs ^= prev_output_mask;
	pending_event = 0;

   	if(state == ACTIVE)
	{
		if(active_outputs == 0)
        {
            debug_printf("going to sleep:%x\n", this);
            state = ASLEEP;
            sleep_time = circuit->global_time;
            return;
        }
        
        pending_event = circuit->queue_push(this, output_events[current_output_event++]);
		if(current_output_event == output_events.size()) current_output_event = first_output_event;
	}
}


void Chip::deactivate_outputs()
{
    //visited = true;

    if(state == ASLEEP)
    {
        wake_up();
    }

    debug_printf("deactivating: %x t:%lld in:%d\n", this, circuit->global_time, inputs);


    if(state == ACTIVE) // Reconnect to inputs
    {
        if((pending_event > circuit->global_time) && ((pending_event - circuit->global_time) <= delay[output]))
        {
            if((pending_event - circuit->global_time) == delay[output])
            {
                debug_printf("super awkward deactivation t:%lld %lld %p\n",  circuit->global_time, pending_event - circuit->global_time, this);

                // Determine if deactivation is due to an input to this chip, which will still have a pending event,
                // in which case no event should be scheduled by this chip. Otherwise it is due to a chip further down stream,
                // in which case the event should stay scheduled. TODO: is this correct?
                bool is_input = false;
                
                for(int i = 0; i < input_links.size(); i++)
                {
                    if(input_links[i].chip->pending_event == circuit->global_time)
                    {
                        is_input = true;
                        debug_printf("is input = TRUE, c:%p\n", input_links[i].chip);
                        break;
                    }
                }

                if(is_input)
                {
                    pending_event = 0;
                    inputs = output ? prev_output_mask : 0;
                }
                else
                {
                    inputs = output ? 0 : prev_output_mask;
                }
            }
            else
            {            
                debug_printf("awkward deactivation t:%lld %lld %p %d\n",  circuit->global_time, pending_event - circuit->global_time, this, delay[output]);
                inputs = output ? 0 : prev_output_mask;
            }
        }
        else
        {
            pending_event = 0;
            inputs = output ? prev_output_mask : 0;
        }

        state = PASSIVE;
        
        active_outputs = (1 << output_links.size()) - 1;

        for(int i = 0; i < input_links.size(); i++)
        {
            input_links[i].chip->active_outputs |= input_links[i].mask;
            if(input_links[i].chip->state == ASLEEP)
                input_links[i].chip->wake_up();

            inputs |= input_links[i].chip->output << i;
        }
        // Reconnecting to clock - set prev input to value of current input
        if(inputs & prev_input_mask) inputs |= prev_input_mask;

        debug_printf("inputs_after:%d\n", inputs);
    }

    output_events.clear();
	current_output_event = 0;

    for(int i = 0; i < INITIAL_STATE_LENGTH; i++)
    {
	    initial_inputs[i] = inputs;
	    initial_time[i] = circuit->global_time;
    }

    cycle_time = 0;
    if(dynamic) active_inputs = 0;
    event_count = 0;

    // TODO: use iterator?
    for(int i = 0; i < output_links.size(); i++)
    {
        if((output_links[i].chip->active_inputs & output_links[i].mask) && output_links[i].chip->event_count)
            output_links[i].chip->deactivate_outputs();
    }
    
    //visited = false;
}


bool Chip::activation_check(uint64_t min_time, uint64_t max_time)
{
    //debug_printf("activation check new: %p act in:%d min:%lld max:%lld\n", this, active_inputs, min_time, max_time);
    
    if(type == CUSTOM_CHIP)
    {
        return false;
    }

    visited = true;      

    const uint64_t global_time = circuit->global_time;

    //for(int i = 0; i < INITIAL_STATE_LENGTH; i++)
    {
    //    debug_printf("i:%d in:%d init_t:%lld init_in:%d\n", i, inputs, initial_time[i], initial_inputs[i]);
    }

    //for(int i = event_count & 1; i < INITIAL_STATE_LENGTH-1; i += 2)
    for(int i = 0; i < INITIAL_STATE_LENGTH-1; i++)
    {
        //if(min_time > initial_time[i+1])
        //    continue;
        
        if(initial_time[i] == global_time) // TODO: change when initial_time is updated in update_inputs instead?
            break;

        if(inputs != initial_inputs[i])
            continue;

        uint64_t min = (min_time > initial_time[i]) ? min_time : initial_time[i];
        uint64_t max = (max_time < initial_time[i+1]) ? max_time : initial_time[i+1];

        if(max < min)
            break;

        bool result = true;

        activation_time = min;

        for(int j = 0; active_inputs >> j; j++)
        {
            j += Chip::next_bit(active_inputs >> j);
            Chip* c = input_links[j].chip;

            if(last_input_event_time[j] < min && dynamic)
                continue;

            if(c->state == ACTIVE && c->cycle_time) // TODO: when is cycle time == 0 ?
            {
                if(c->activation_time > min)
                    min = c->activation_time;
            
                if(min > max)
                {
                    result = false;
                    break;
                }
                
                activation_time = min + ((global_time - min) % c->cycle_time);

                if(activation_time > max || (activation_time == max && max > min))
                {
                    /*if(activation_time > last_input_event_time[j])
                    {
                        if(last_input_event_time[j] > min_time)
                            min_time = last_input_event_time[j];
                    }
                    else
                        min_time = activation_time;*/
                    
                    result = false;
                    break;
                }

                min = max = activation_time;
            }
            else if(c->state == PASSIVE && c->visited == false) 
            {
                result = c->activation_check(min, max);
                if(result == false)
                {
                    break;
                }
                else 
                {
                    min = max = activation_time = c->activation_time;
                }
            }
        }
        
        if(result == true)
        {
            offset_time = activation_time - initial_time[0];
            first_output_event = 0;
        
            while(offset_time)
            {
                if(first_output_event == output_events.size()) break;
        
                if(output_events[first_output_event] <= offset_time) // TODO: Sure about == cases?
                    offset_time -= output_events[first_output_event++];
                else
                    break;
            }

            if((output_events.size() - first_output_event) & 1) // Odd # of output events, don't activate
                continue;

            // Fix cases where propagation delays have been modified, i.e., 555 mono timers? Hack?
            if(output_events.size() != first_output_event &&
              cycle_time >= output_events[first_output_event] + global_time - activation_time)
                continue;

            debug_printf("act_check3:%p offs:%lld first:%d\n", this, offset_time, first_output_event);

            //debug_printf("activation check new: %p result:%d\n", this, (int)result);
            
            visited = false;
            return true;
        }
    }

    //debug_printf("activation check new: %p result:%d\n", this, (int)false);

    visited = false;
    return false;
}

void Chip::activate_inputs()
{
    //visited = true;

    activation_count++;

    debug_printf("activating: %x %d t:%lld act:%lld\n", this, active_inputs, circuit->global_time, activation_time);

    state = ACTIVE;

    for(int i = 0; active_inputs >> i; i++)
    {
        i += Chip::next_bit(active_inputs >> i);
        Chip* c = input_links[i].chip;

        if(last_input_event_time[i] < activation_time && dynamic)
            continue;

        debug_printf("disconnecting %x from %x mask %d\n", this, c, input_links[i].mask);

        c->active_outputs &= ~input_links[i].mask;

        if(c->state == PASSIVE /*&& c->visited == false*/)
        {
            //c->activation_time = activation_time;
            c->activate_inputs();
        }
    }

    debug_printf("out ev before: %d\n", output_events.size());
    for(int i = 0; i < output_events.size(); i++)
        debug_printf("%lld ", output_events[i]);
    debug_printf("\n");

    cycle_time -= activation_time - initial_time[0] - offset_time;

    if(first_output_event != 0) debug_printf("first_out:%d time:%lld\n", first_output_event, time);

    if(first_output_event != output_events.size())
    {
        if(pending_event)
        {
            current_output_event = first_output_event;
            debug_printf("activation2 %p t:%lld %lld %lld %lld\n", this, circuit->global_time, pending_event - circuit->global_time, delay[0], delay[1]);
        }
        else
        {
            pending_event = circuit->queue_push(this, output_events[first_output_event] - offset_time);
            current_output_event = first_output_event + 1;
            debug_printf("activation3 %p t:%lld %lld\n", this, circuit->global_time, pending_event);
        }

        output_events[first_output_event] += circuit->global_time - cycle_time - activation_time; //TODO: correct???
        cycle_time = circuit->global_time - activation_time; 
    }
    //else
    {
    //    cycle_time = 1;
    }

    debug_printf("cyc: %lld\n", cycle_time);

    debug_printf("out ev %x: %d %d\n", this, first_output_event, output_events.size());
    for(int i = 0; i < output_events.size(); i++)
        debug_printf("%lld ", output_events[i]);
    debug_printf("\n");

    //TODO: Is this working 100%? Does it even help anymore?
    //activate_outputs();

    //visited = false;
}

void Chip::make_dynamic()
{
    dynamic = true;
    MAX_INPUT_EVENTS = 4096;
    MAX_CYCLE_TIME = 1.0e-3 / Circuit::timescale; // 1 ms

    for(int i = 0; i < output_links.size(); i++)
        if(!output_links[i].chip->dynamic)
            output_links[i].chip->make_dynamic();
}

#if 0
void Chip::activate_outputs()
{
    debug_printf("activate outputs:%p\n", this);
    
    for(int i = 0; active_outputs >> i; i++)
    {
        i += Chip::next_bit(active_outputs >> i);
        Chip* c = output_links[i].chip;
        if(/*c->visited == false && */ c->state == PASSIVE && (c->active_inputs & output_links[i].mask) && c->activation_check(activation_time, activation_time))
        {
            c->activate_inputs();
            c->activate_outputs();
        }
    }
}
#endif
