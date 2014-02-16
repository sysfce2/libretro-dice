#include "circuit.h"
#include "circuit_desc.h"

#include <map>
#include <string>
#include <cstdio>

#define DEBUG
#undef DEBUG

#define EVENT_QUEUE_SIZE 128
#define SUBCYCLE_SIZE 64

CHIP_DESC( _VCC ) = 
{
	CUSTOM_CHIP_START(NULL)
	OUTPUT_PIN(1)
};

CHIP_DESC( _GND ) = 
{
	CUSTOM_CHIP_START(NULL)
	OUTPUT_PIN(1)
};

// Option to disable optimizations
// on chips where they do not perform well
CUSTOM_LOGIC( deoptimize )
{
    for(ChipLink& cl : chip->output_links)
    {
        printf("Deoptimizing %p\n", cl.chip);
        cl.chip->optimization_disabled = true;
    }
}

CHIP_DESC( _DEOPTIMIZER ) = 
{
	CUSTOM_CHIP_START(deoptimize)
	OUTPUT_PIN(1)
};

const double Circuit::timescale = 1.0e-12; // 1 ps

Circuit::Circuit(Settings& s, Input& i, Video& v, const CircuitDesc* desc, const char* name) 
    : settings(s), game_config(desc, name), input(i), video(v), global_time(0), queue_size(0)
{
	typedef std::pair<Chip*, ChipDesc*> ChipDescPair;
	typedef std::pair<uint8_t, ChipDescPair> Connection;
    typedef std::pair<std::string, uint8_t> Net;
    typedef std::multimap<std::string, ChipDescPair>::iterator ChipMapIterator;
    typedef std::multimap<std::string, Net>::iterator NetListIterator;

    std::multimap<std::string, ChipDescPair> chip_map;
    std::multimap<std::string, Net> net_list;
    std::vector<Connection> connection_list_out, connection_list_in;

    const CircuitDesc* desc_start = desc;

    // Construct special chips
    chips.push_back(new Chip(1, 64, this, chip__VCC));
    chip_map.insert( std::pair<std::string, ChipDescPair>("_VCC", ChipDescPair(chips.back(), chip__VCC)) );

	chips.push_back(new Chip(1, 64, this, chip__GND));
    chip_map.insert( std::pair<std::string, ChipDescPair>("_GND", ChipDescPair(chips.back(), chip__GND)) );

	chips.push_back(new Chip(1, 64, this, chip__DEOPTIMIZER));
    chip_map.insert( std::pair<std::string, ChipDescPair>("_DEOPTIMIZER", ChipDescPair(chips.back(), chip__DEOPTIMIZER)) );

	// Create Video chip TODO: TEMP HACK?
	{
        std::map<uint8_t, ChipDescPair> output_pin_map;
        int chip = chips.size();

		for(ChipDesc* d = chip_VIDEO; !d->endOfDesc(); d++)
		{
    		chips.push_back(new Chip(8, 64, this, d, &video));

			ChipDescPair cd(chips.back(), d);
            chip_map.insert( std::pair<std::string, ChipDescPair>("VIDEO", cd) );

            if(d->output_pin) 
                output_pin_map[d->output_pin] = cd;
		}

        // Create list of internal connections
        for(ChipDesc* d = chip_VIDEO; !d->endOfDesc(); d++, chip++)
            for(int i = 0; d->input_pins[i]; i++)
                if(output_pin_map.count(d->input_pins[i]))
                {
                    connection_list_out.push_back(*output_pin_map.find(d->input_pins[i]));
                    connection_list_in.push_back(Connection(d->input_pins[i], ChipDescPair(chips[chip], d)));
                }
	}

	// Create Audio chip TODO: TEMP HACK?
	{
        std::map<uint8_t, ChipDescPair> output_pin_map;
        int chip = chips.size();

		for(ChipDesc* d = chip_AUDIO; !d->endOfDesc(); d++)
		{
    		chips.push_back(new Chip(8, 64, this, d, &audio));

			ChipDescPair cd(chips.back(), d);
            chip_map.insert( std::pair<std::string, ChipDescPair>("AUDIO", cd) );

            if(d->output_pin) 
                output_pin_map[d->output_pin] = cd;
		}

        // Create list of internal connections
        for(ChipDesc* d = chip_AUDIO; !d->endOfDesc(); d++, chip++)
            for(int i = 0; d->input_pins[i]; i++)
                if(output_pin_map.count(d->input_pins[i]))
                {
                    connection_list_out.push_back(*output_pin_map.find(d->input_pins[i]));
                    connection_list_in.push_back(Connection(d->input_pins[i], ChipDescPair(chips[chip], d)));
                }
	}


    // Search for simulator optimization hints
    std::map<const char*, OptimizationHintDesc> hint_list;
    for(desc = desc_start; desc->type != CIRCUIT_END; desc++)
    {
        if(desc->type == OPTIMIZATION_HINT)
        {
            printf("Hinting %s\n", desc->u.hint.chip);
            hint_list[desc->u.hint.chip] = desc->u.hint;
        }
    }

	// Find and construct all chips
    //std::map<ChipDesc*, Chip*> desc_map; // Doesn't work with custom chips, TODO: fix
	for(desc = desc_start; desc->type != CIRCUIT_END; desc++)
	{
		//std::map<ChipDesc*, Chip*> desc_map;
        
        if(desc->type == CHIP_INST)
		{
            const ChipInstance& instance = desc->u.instance;
            std::map<uint8_t, ChipDescPair> output_pin_map;
            int chip = chips.size();
            
			for(ChipDesc* d = instance.chip; !d->endOfDesc(); d++)
			{
                // Chip LUT already created, so just make a copy
				/*if(desc_map.count(d))
				{
					chips.push_back(new Chip(*desc_map.find(d)->second));
				}
				else*/
				{
					if(hint_list.find(instance.name) != hint_list.end())
                        chips.push_back(new Chip(hint_list[instance.name].queue_size, hint_list[instance.name].subycle_size, this, d, (void*)instance.custom_data));   
                    else // Use defaults
                        chips.push_back(new Chip(EVENT_QUEUE_SIZE, SUBCYCLE_SIZE, this, d, (void*)instance.custom_data));

                    #ifdef DEBUG
                    printf("chip name:%s p:%p\n", instance.name, chips.back());
                    #endif
                    //desc_map[d] = chips.back();
				}

				ChipDescPair cd(chips.back(), d);
                chip_map.insert( std::pair<std::string, ChipDescPair>(instance.name, cd) );

                if(d->output_pin) 
                    output_pin_map[d->output_pin] = cd;
			}

            // Create list of internal connections
            for(ChipDesc* d = instance.chip; !d->endOfDesc(); d++, chip++)
                for(int i = 0; d->input_pins[i]; i++)
                    if(output_pin_map.count(d->input_pins[i]))
                    {
                        connection_list_out.push_back(*output_pin_map.find(d->input_pins[i]));
                        connection_list_in.push_back(Connection(d->input_pins[i], ChipDescPair(chips[chip], d)));
                    }
		}
	}

    // Create list of nets
    for(desc = desc_start; desc->type != CIRCUIT_END; desc++)
    {
        if(desc->type == NET)
        {
            Net n = { desc->u.net.chip, desc->u.net.pin };
            net_list.insert(std::pair<std::string, Net>(desc->u.net.name, n));
        }
    }

    // Convert nets to connections
    for(NetListIterator it1 = net_list.begin(); it1 != net_list.end();)
    {
        NetListIterator it2 = net_list.upper_bound(it1->first);
        Connection output;

        // Find output
        for(NetListIterator it = it1; it != it2; ++it)
        {
            if(chip_map.count(it->second.first) == 0)
            {
                printf("WARNING: Cannot connect non-existant chip %s to net %s\n", it->second.first.c_str(), it->first.c_str());
                continue;
            }           

            for(ChipMapIterator cmi = chip_map.lower_bound(it->second.first); cmi != chip_map.upper_bound(it->second.first); ++cmi)
                if(it->second.second == cmi->second.second->output_pin)
                {
                    if(output.first != 0) // Output already found
                        printf("WARNING: Net %s contains multiple output ports, skipping output %s.%d\n", it->first.c_str(), it->second.first.c_str(), it->second.second);
                    else
                        output = Connection(it->second.second, cmi->second);

                    break;
                }
        }
        
        if(output.first == 0) // Output not found
        {
            printf("WARNING: Net %s contains no output ports\n", it1->first.c_str());
        }
        else for(NetListIterator it = it1; it != it2; ++it) // Create connections from output to all inputs
        {
            for(ChipMapIterator cmi = chip_map.lower_bound(it->second.first); cmi != chip_map.upper_bound(it->second.first); ++cmi)
                for(int i = 0; cmi->second.second->input_pins[i]; i++)
                    if(it->second.second == cmi->second.second->input_pins[i])
                    {
                        connection_list_out.push_back(output);
                        connection_list_in.push_back(Connection(it->second.second, cmi->second));
                    }
        }
            
        it1 = it2;
    }
    

    // Create list of connections
    for(desc = desc_start; desc->type != CIRCUIT_END; desc++)
	{
        if(desc->type == CONNECTION)
        {
            const ConnectionDesc& connection = desc->u.connection;
            ChipMapIterator it1, it2;
            std::pair<ChipMapIterator, ChipMapIterator> range1, range2;

            range1 = chip_map.equal_range(connection.name1);
            range2 = chip_map.equal_range(connection.name2);

            // Find output pin
            bool connected = false;
            for(it1 = range1.first; it1 != range1.second; ++it1)
                if(it1->second.second->output_pin == connection.pin1)
                {
                    for(it2 = range2.first; it2 != range2.second; ++it2)
                        for(int i = 0; it2->second.second->input_pins[i]; i++)
                            if(it2->second.second->input_pins[i] == connection.pin2)
                            {
                                if(std::find(connection_list_in.begin(), connection_list_in.end(), Connection(connection.pin2, it2->second)) != connection_list_in.end())
                                {
                                    printf("WARNING: Attempted multiple connections to input: %s.%d\n", connection.name2, connection.pin2);
                                }
                                //else
                                {
                                    connected = true;
                                    connection_list_out.push_back(Connection(connection.pin1, it1->second));
                                    connection_list_in.push_back(Connection(connection.pin2, it2->second));
                                }
                            }
                    break;
                }

            for(it2 = range2.first; it2 != range2.second; ++it2)
                if(it2->second.second->output_pin == connection.pin2)
                {
                    for(it1 = range1.first; it1 != range1.second; ++it1)
                        for(int i = 0; it1->second.second->input_pins[i]; i++)
                            if(it1->second.second->input_pins[i] == connection.pin1)
                            {
                                if(std::find(connection_list_in.begin(), connection_list_in.end(), Connection(connection.pin1, it1->second)) != connection_list_in.end())
                                {
                                    printf("WARNING: Attempted multiple connections to input: %s.%d\n", connection.name1, connection.pin1);
                                }
                                //else
                                {
                                    connected = true;
                                    connection_list_out.push_back(Connection(connection.pin2, it2->second));
                                    connection_list_in.push_back(Connection(connection.pin1, it1->second));
                                }
                            }
                    break;
                }

            if(!connected)
                printf("WARNING: Invalid connection: %s.%d -> %s.%d\n", connection.name1, connection.pin1,
                                                                        connection.name2, connection.pin2);
        }
    }

    // Delete all chips w/ unconnected outputs
    bool removed;
    do 
    { 
        removed = false;
        for(std::vector<Chip*>::iterator it = chips.begin(); it != chips.end(); ++it)
        {
            // Skip custom chips
            if((*it)->type == CUSTOM_CHIP) continue;
            
            bool found = false;
            for(std::vector<Connection>::iterator it2 = connection_list_out.begin(); it2 != connection_list_out.end(); ++it2)
                if(*it == it2->second.first)
                {
                    found = true;
                    break;
                }
    
            if(!found)
            {
                for(auto x : chip_map)
                    if(x.second.first == *it)
                    {
                        printf("Removing unused chip %s.%d\n", x.first.c_str(), x.second.second->output_pin);
                        break;
                    }

                removed = true;
                
                // Remove all connections to this chip
                for(int i = 0; i < connection_list_in.size(); i++)
                    if(*it == connection_list_in[i].second.first)
                    {
                        connection_list_in.erase(connection_list_in.begin() + i);
                        connection_list_out.erase(connection_list_out.begin() + i);
                        i--;
                    }
                
                if((*it)->type == BASIC_CHIP)
                    delete[] (*it)->lut;
 
                delete *it;
                it = chips.erase(it) - 1;
            }
        }
    } while(removed);

    // Make all connections
    for(int i = 0; i < connection_list_out.size(); i++)
    {
        Chip* c_out    = connection_list_out[i].second.first;
        Chip* c_in     = connection_list_in[i].second.first;
        ChipDesc* desc = connection_list_in[i].second.second;
        uint8_t pin    = connection_list_in[i].first;
        
        c_out->connect(c_in, desc, pin);

        if(c_out->output_links.size() > 64) 
        {
            std::string name = "";
            
            for(auto x : chip_map)
                if(x.second.first == c_out)
                {
                    name = x.first;
                    break;
                }
            
            if(name != "_VCC" && name != "_GND")
                printf("ERROR: Maximum output connection limit reached, chip:%s, cout:%d\n", name.c_str(), c_out->output_links.size());
        }
    }


    // Check for unconnected inputs, connect to GND
    for(int i = 2; i < chips.size(); i++)
        for(int j = 0; j < chips[i]->input_links.size(); j++)
            if(chips[i]->input_links[j].chip == NULL)
            {
                std::string name = "";
                int pin = 0;

                for(auto x : chip_map)
                    if(x.second.first == chips[i])
                    {
                        name = x.first;
                        pin = x.second.second->input_pins[j];
                        break;
                    }
                
                if(chips[i]->type != CUSTOM_CHIP)
                    printf("WARNING: Unconnected input pin: %s.%d, connecting to GND\n", name.c_str(), pin);
                
                chips[1]->output_links.push_back(ChipLink(chips[i], 1 << j));
                chips[i]->input_links[j] = ChipLink(chips[1], 0);
            }


    // Find video descriptor
    video.desc = &VideoDesc::DEFAULT;
    for(desc = desc_start; desc->type != CIRCUIT_END; desc++)
        if(desc->type == VIDEO_INST)
            video.desc = desc->u.video;

    // Set up VCC & GND for analog
    chips[0]->analog_output = 5.0;
    chips[1]->analog_output = 0.0;

    // Find audio descriptor
    audio.desc = NULL;
    for(desc = desc_start; desc->type != CIRCUIT_END; desc++)
        if(desc->type == AUDIO_INST)
            audio.desc = desc->u.audio;

    audio.audio_init(this);

    // Run VCC
    chips[0]->output = 1;
    for(LinkIterator it = chips[0]->output_links.begin(); it != chips[0]->output_links.end(); ++it)
		it->chip->inputs |= it->mask;

	for(int i = 2; i < chips.size(); i++)
		chips[i]->initialize();
}

Circuit::~Circuit()
{
    for(std::vector<Chip*>::iterator it = chips.begin(); it != chips.end(); ++it)
    {
        if((*it)->type == BASIC_CHIP)
            delete[] (*it)->lut;

        delete *it;
    }
}

uint64_t Circuit::queue_push(Chip* chip, uint64_t delay)
{
    int i;
	QueueEntry qe(global_time + delay, chip);

    queue_size++;

    for(i = queue_size; i > 1 && queue[i >> 1].time > qe.time; i >>= 1)
        queue[i] = queue[i >> 1];

    queue[i] = qe;

	return qe.time;
}


void Circuit::queue_pop()
{
    QueueEntry qe = queue[queue_size];

	int i = 1;
    queue_size--;

    while((i << 1) < (queue_size + 1))
    {
        int child = (i << 1);

        if(child + 1 < queue_size + 1 && queue[child + 1].time < queue[child].time)
            child++;

        if(qe.time <= queue[child].time)
            break;

        queue[i] = queue[child];
        i = child;
    }

    queue[i] = qe;
}

void Circuit::run(int64_t run_time)
{
    while(run_time > 0)
	{        
        if(queue_size)
        {
            run_time -= queue[1].time - global_time;
		    global_time = queue[1].time;
        }
        else
		{
			global_time += run_time;	
			return;
		}

		if(global_time == queue[1].chip->pending_event)
		{	
			queue[1].chip->update_output();
		}
        queue_pop();
	}
}

