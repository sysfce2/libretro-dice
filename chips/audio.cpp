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

    chip->output_events.clear();
    chip->output_events.push_back(chip->circuit->global_time);
    chip->output_events.push_back(chip->circuit->global_time + chip->delay[0]);
    chip->cycle_time = chip->delay[0] + chip->delay[1];
    chip->first_output_event = chip->output_events.begin();
    chip->current_output_event = chip->output_events.begin();
    chip->end_time = ~0ull;

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

Audio::Audio() : last_sample_time(0), sample_period(0.0), gain(1 << 8)
{ }

void Audio::audio_init(Circuit* circuit)
{
    
    static const int FREQUENCY[] = { 22050, 44100, 48000, 96000 }; 
    //int buffer_size = FREQUENCY[circuit->settings.audio.frequency] / 50; // 20 ms, TODO: Make configurable?
    // Hardcoded 2048 buffer size now TODO: Make configurable?
    int buffer_size = 2048;

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

    sample_period = uint64_t(1.0 / (Circuit::timescale * as.freq));
    last_sample_time = circuit->global_time;

    SDL_PauseAudio(circuit->settings.audio.mute);

    // Initialize gain array
    if(desc)
    {
        for(int i = 0; i < gain.size(); i++)
        {
            double r_hi = 0.0, r_lo = 0.0;

            for(int j = 0; j < 8; j++)
            {
                if(desc->r[j] < 1.0) continue; // Assume 1 Ohm minimum

                if(i & (1 << j))
                    r_hi += 1.0 / desc->r[j];
                else
                    r_lo += 1.0 / desc->r[j];
            }

            double val;
            if(r_lo < 1.0e-6)
            {
                val = 1.0;
            }
            else if(r_hi < 1.0e-6)
            {
                val = 0.0;
            }
            else
            {
                r_hi = 1.0 / r_hi;
                r_lo = 1.0 / r_lo;
                val = r_lo / (r_hi + r_lo);
            }

            // Scale (0,1) to (-1,1)
            gain[i] = 2 * val * desc->gain - 1.0;
            //gain[i] = val * desc->gain;

            if(gain[i] > 1.0) gain[i] = 1.0;
            if(gain[i] < -1.0) gain[i] = -1.0;

            //printf("Gain %d: %g %g %g %g\n", i, r_lo, r_hi, val, gain[i]);
        }
    }
    else
    {
        gain[0] = -1.0;

        // Any input high = max gain
        for(int i = 1; i < gain.size(); i++)
            gain[i] = 1.0;
    }
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

    SDL_LockAudio();
    while(audio->last_sample_time + audio->sample_period < chip->circuit->global_time)
    {
        //audio->audio_buffer.push_back(chip->inputs ? vol : -vol);
        audio->audio_buffer.push_back(audio->gain[chip->inputs] * vol);
        audio->last_sample_time += audio->sample_period;
    }
    SDL_UnlockAudio();

    chip->inputs ^= mask;
}

void Audio::callback(void* userdata, uint8_t* str, int len)
{
    Audio* audio = (Audio*)userdata;
    cirque<int16_t, 4096>& buffer = audio->audio_buffer;
    
    int16_t* stream = (int16_t*)str;
    int length = len >> 1;
    
    static int16_t last_val = 0;

    for(int i = 0; i < length; i++)
    {
        if(!buffer.empty())
        {
            last_val = stream[i] = buffer.front();
            buffer.pop_front();
        }
        else 
        {
            stream[i] = last_val;
        }
    }
}

