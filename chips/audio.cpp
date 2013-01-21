#include "audio.h"
#include "../circuit.h"

#include <SDL/SDL.h>

/* 
	Inputs:
		1-7: AUDIO

	Outputs:
		None

    TODO: Handle simulation running slower than real time better
    TODO: Audio can cause program to hang? If audio device isn't working properly? 
*/

static CUSTOM_LOGIC( audio_timer )
{
    chip->state = ACTIVE;
    chip->activation_time = chip->circuit->global_time;

    chip->output_events.resize(2);
    chip->output_events[0] = chip->delay[0];
    chip->output_events[1] = chip->delay[1];
    chip->cycle_time = chip->delay[0] + chip->delay[1];
    chip->current_output_event = 0;

    if(chip->output_links.size())
        chip->output_links[0].mask = 0;

    chip->pending_event = chip->circuit->queue_push(chip, chip->delay[0]);
}

CHIP_DESC( AUDIO ) = 
{
	CUSTOM_CHIP_START(&audio_timer)
        OUTPUT_DELAY_MS( 5.0, 5.0 ) // 5 ms poll rate, keeps sample buffer filled
        OUTPUT_PIN( i1 ),
    
    CUSTOM_CHIP_START(&Audio::audio)
	    INPUT_PINS(1, 2, 3, 4, 5, 6, 7, i1),

	CHIP_DESC_END
};

Audio::Audio() : current_buffer(0), current_pos(0), last_sample_time(0), sample_period(0.0)
{ }

void Audio::audio_init(Circuit* circuit)
{
    static const int FREQUENCY[] = { 22050, 44100, 48000, 96000 }; 
    int buffer_size = FREQUENCY[circuit->settings.audio.frequency] / 20; // 50 ms, TODO: Make configurable?

    SDL_AudioSpec as;
	as.freq = FREQUENCY[circuit->settings.audio.frequency];
	as.format = AUDIO_S16SYS;
	as.channels = 1;
	as.samples = buffer_size;
	as.callback = &Audio::callback;
	as.userdata = (void*)this;

    SDL_CloseAudio();

	if(SDL_OpenAudio(&as, NULL) < 0)
	{
		printf("Unable to open audio:\n%s\n", SDL_GetError());
		exit(1);
	}

    audio_buffer[0].resize(as.samples);
    audio_buffer[1].resize(as.samples);
    sample_period = uint64_t(1.0 / (Circuit::timescale * as.freq));

    current_buffer = current_pos = 0;
    last_sample_time = circuit->global_time;

    SDL_PauseAudio(circuit->settings.audio.mute);
}

Audio::~Audio()
{
	SDL_PauseAudio(1); // TODO: move?
	SDL_CloseAudio();
}

CUSTOM_LOGIC( Audio::audio )
{
    Audio* audio = (Audio*)chip->custom_data;

    int16_t vol = 30 * chip->circuit->settings.audio.volume; 

    while(audio->last_sample_time + audio->sample_period < chip->circuit->global_time)
    {
        audio->audio_buffer[audio->current_buffer][audio->current_pos++] = chip->inputs ? vol : 0;
        audio->last_sample_time += audio->sample_period;
        if(audio->current_pos == audio->audio_buffer[audio->current_buffer].size())
        {
            SDL_LockAudio();

            audio->current_buffer ^= 1;
            audio->current_pos = 0;

            SDL_UnlockAudio();
        }        
    }

    chip->inputs ^= mask;
}

void Audio::callback(void* userdata, uint8_t* str, int len)
{
    Audio* audio = (Audio*)userdata;
    std::vector<int16_t>& buffer = audio->audio_buffer[audio->current_buffer ^ 1];
    
    int16_t* stream = (int16_t*)str;
    int length = len >> 1;

    for(int i = 0; i < length; i++)
        stream[i] = buffer[i];
}

