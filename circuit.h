#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <stdint.h>
#include <vector>

#include "settings.h"
#include "game_config.h"

#include "chip.h"
#include "realtime.h"
#include "chips/video.h"
#include "chips/audio.h"
#include "chips/input.h"

#define MAX_QUEUE_SIZE 1024

class CircuitDesc;

struct QueueEntry
{
	uint64_t time;
	Chip* chip;

    QueueEntry(uint64_t t = 0, Chip* c = NULL) : time(t), chip(c) { }
};

class Circuit
{
public:
	std::vector<Chip*> chips;
    uint64_t global_time;

    Settings& settings; // TODO: make const?
    GameConfig game_config;
    Input& input;
    Video video;
    Audio audio;
    RealTimeClock rtc;
   
    int queue_size;
    QueueEntry queue[MAX_QUEUE_SIZE];

	Circuit(Settings& s, Input& i, const CircuitDesc* desc, const char* name);
    ~Circuit();

	uint64_t queue_push(Chip* chip, uint64_t delay);
    void queue_pop();
	void run(int64_t time);

	static const double timescale;
};

#endif
