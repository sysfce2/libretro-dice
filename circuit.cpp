#include "circuit.h"
#include "circuit_desc.h"

#include <map>
#include <string>
#include <cstdio>

CHIP_DESC( _VCC ) = 
{
	CUSTOM_CHIP_START(NULL)
	OUTPUT_PIN(0)
};

CHIP_DESC( _GND ) = 
{
	CUSTOM_CHIP_START(NULL)
	OUTPUT_PIN(0)
};

extern CIRCUIT_LAYOUT( stuntcycle );

const double Circuit::timescale = 1.0e-12; // 1 ps

Circuit::Circuit(Settings& s, Input& i, const CircuitDesc* desc, const char* name) 
    : settings(s), game_config(desc, name), input(i), global_time(0), queue_size(0)
{
	typedef std::pair<Chip*, ChipDesc*> ChipDescPair;
	typedef std::pair<uint8_t, ChipDescPair> Connection;
    typedef std::multimap<std::string, ChipDescPair>::iterator ChipMapIterator;

    std::multimap<std::string, ChipDescPair> chip_map;
    std::vector<Connection> connection_list_out, connection_list_in;

    const CircuitDesc* desc_start = desc;

    // Construct special chips
    chips.push_back(new Chip(this, chip__VCC));
    chip_map.insert( std::pair<std::string, ChipDescPair>("_VCC", ChipDescPair(chips.back(), chip__VCC)) );

	chips.push_back(new Chip(this, chip__GND));
    chip_map.insert( std::pair<std::string, ChipDescPair>("_GND", ChipDescPair(chips.back(), chip__GND)) );

	// Create Video chip TODO: TEMP HACK?
	{
        std::map<uint8_t, ChipDescPair> output_pin_map;
        int chip = chips.size();

		for(ChipDesc* d = chip_VIDEO; !d->endOfDesc(); d++)
		{
    		chips.push_back(new Chip(this, d, &video));

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
    		chips.push_back(new Chip(this, d, &audio));

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
					chips.push_back(new Chip(this, d, instance.custom_data));
                    //desc_map[d] = chips.back();
                    
                    // For some circuits, use static/dynamic optimization, which allows
                    // longer cycles for static logic (driven only by clock).
                    if(desc_start == circuit_stuntcycle)
                    {
                        chips.back()->dynamic = false;
                        chips.back()->MAX_INPUT_EVENTS = 524288;
                        chips.back()->MAX_CYCLE_TIME = 50.0e-3 / 1.0e-12;
                    }
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
                                else
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
                                else
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
                printf("Removing unused chip %p\n", *it);
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
                    delete (*it)->lut;
    
                delete *it;
                it = chips.erase(it);
                --it;
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
    video.desc = NULL;
    for(desc = desc_start; desc->type != CIRCUIT_END; desc++)
        if(desc->type == VIDEO_INST)
            video.desc = desc->u.video;


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
            delete (*it)->lut;

        delete *it;
    }
}

uint64_t Circuit::queue_push(Chip* chip, uint64_t delay)
{
    int i;
	QueueEntry qe(chip->circuit->global_time + delay, chip);

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

