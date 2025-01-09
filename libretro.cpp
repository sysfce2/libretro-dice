#include <sstream>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include <stdio.h>
#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>
#endif
#include "libretro.h"
#include "dice.h"

static uint8_t *frame_buf;
static struct retro_log_callback logging;
static retro_log_printf_t log_cb;
static bool use_audio_cb;
static float last_aspect;
static float last_sample_rate;
char retro_base_directory[4096];
char retro_game_path[4096];

static dice_libretro::DICE dice;

static void fallback_log(enum retro_log_level level, const char *fmt, ...)
{
   (void)level;
   va_list va;
   va_start(va, fmt);
   vfprintf(stderr, fmt, va);
   va_end(va);
}


static retro_environment_t environ_cb;

static unsigned color;  // TODO (kmitton)

void retro_init(void)
{
   int safety_factor = 2;
   frame_buf = (uint8_t*)malloc(VIDEO_PIXELS * VIDEO_BYTES_PER_PIXEL * safety_factor);
   uint16_t *pixel_buffer = reinterpret_cast<uint16_t *>(frame_buf);
   for (unsigned i = 0; i<VIDEO_PIXELS; i++) {
      pixel_buffer[i] = 0; // 0x2222;
   }
   const char *dir = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &dir) && dir)
   {
      snprintf(retro_base_directory, sizeof(retro_base_directory), "%s", dir);
   }
   color = 0;
}

void retro_deinit(void)
{
   free(frame_buf);
   frame_buf = NULL;
}

unsigned retro_api_version(void)
{
   return RETRO_API_VERSION;
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
   log_cb(RETRO_LOG_INFO, "Plugging device %u into port %u.\n", device, port);
}

void retro_get_system_info(struct retro_system_info *info)
{
   memset(info, 0, sizeof(*info));
   info->library_name     = "dice";
   info->library_version  = "0.1.0";
   info->need_fullpath    = true;
   info->valid_extensions = "zip|dmy";
   info->block_extract = true;
}

static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;

void retro_get_system_av_info(struct retro_system_av_info *info)
{
   float aspect                = 4.0f / 3.0f;
   float sampling_rate         = 30000.0f;

   info->timing = (struct retro_system_timing) {
      .fps = 8.0,
      .sample_rate = 0.0,
   };
   info->geometry.base_width   = VIDEO_WIDTH;
   info->geometry.base_height  = VIDEO_HEIGHT;
   info->geometry.max_width    = VIDEO_WIDTH;
   info->geometry.max_height   = VIDEO_HEIGHT;
   info->geometry.aspect_ratio = aspect;

   last_aspect                 = aspect;
   last_sample_rate            = sampling_rate;
}

void retro_set_environment(retro_environment_t cb)
{
   environ_cb = cb;

   if (cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &logging))
      log_cb = logging.log;
   else
      log_cb = fallback_log;

   static const struct retro_controller_description controllers[] = {
      { "Arcade stick", RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_ANALOG, 0) },
   };

   static const struct retro_controller_info ports[] = {
      { controllers, 1 },
      { NULL, 0 },
   };

   cb(RETRO_ENVIRONMENT_SET_CONTROLLER_INFO, (void*)ports);
   
   static const struct retro_variable vars[] = {
      { NULL, NULL },
   };

   cb(RETRO_ENVIRONMENT_SET_VARIABLES, (void*)vars);
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{
   audio_cb = cb;
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
   audio_batch_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb)
{
   input_poll_cb = cb;
}

void retro_set_input_state(retro_input_state_t cb)
{
   input_state_cb = cb;
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
   video_cb = cb;
}

void retro_reset(void)
{
   // TODO (kmitton): Read level and 4-player inputs before restarting initialization.
   // check_variables();

   dice.reset();
}

static void update_input(void)
{
   int32_t input_bitmask[NUM_CONTROLLERS];
   int32_t input_analog_left_x[NUM_CONTROLLERS];
   int32_t input_analog_left_y[NUM_CONTROLLERS];

   for (unsigned pad = 0; pad < NUM_CONTROLLERS; pad++)
   {
      input_bitmask[(pad)] = 0;
      for (int i = 0; i <= RETRO_DEVICE_ID_JOYPAD_R3; i++) \
         input_bitmask[(pad)] |= input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, i) ? (1 << i) : 0 ;
      
      input_analog_left_x[pad] = input_state_cb( (pad), RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT,
                                         RETRO_DEVICE_ID_ANALOG_X);
      
      input_analog_left_y[pad] = input_state_cb( (pad), RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT,
                                         RETRO_DEVICE_ID_ANALOG_Y);
      
      //printf("KAM2 input_bitmask %u %08X %08X %08X\n", pad, input_bitmask[(pad)], input_analog_left_x[(pad)], input_analog_left_y[(pad)]);

   }
   dice.update_input(input_bitmask, input_analog_left_x, input_analog_left_y);
}

static void check_variables(void)
{
}

static void audio_callback(void)
{
   // TODO (kmitton): TAUNT?
}

static void audio_set_state(bool enable)
{
   (void)enable;
}

typedef uint16_t pixel_t;

void retro_run(void)
{
   update_input();

#ifdef DEBUG2
   printf("KAM0 retro_run\n");
#endif

   dice.run();
   //dice.render_frame();

   // Show screen.
   video_cb(frame_buf, VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_PITCH);
   
   // Wipe screen.
   uint16_t *pixel_buffer = reinterpret_cast<uint16_t *>(frame_buf);
   for (unsigned i = 0; i<VIDEO_PIXELS; i++) {
      pixel_buffer[i] = 0; // 0x2222;
   }

   bool updated = false;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
      check_variables();
}

bool retro_load_game(const struct retro_game_info *info)
{
#ifdef DEBUG
   printf("KAM50 retro_load_game\n");
#endif

   struct retro_input_descriptor desc[] = {
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "Left" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "Up" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "Down" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Right" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Coin" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START,  "Start" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,  "Button 1" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,  "Button 2" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,  "Button 3" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,  "Dollar" },

      { 0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X, "Left Analog X" },
      { 0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y, "Left Analog Y" },
      { 0 },
   };

   environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, desc);

   enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_RGB565;
   if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
   {
      log_cb(RETRO_LOG_INFO, "RGB565 is not supported.\n");
      return false;
   }

   snprintf(retro_game_path, sizeof(retro_game_path), "%s", info->path);
   struct retro_audio_callback audio_cb = { audio_callback, audio_set_state };
   use_audio_cb = environ_cb(RETRO_ENVIRONMENT_SET_AUDIO_CALLBACK, &audio_cb);

   check_variables();

   uint16_t *pixel_buffer = reinterpret_cast<uint16_t *>(frame_buf);
   dice.load_game(info->path, pixel_buffer);

   (void)info;

   return true;
}

void retro_unload_game(void)
{

}

unsigned retro_get_region(void)
{
   return RETRO_REGION_NTSC;
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num)
{
   return false;
}

size_t retro_serialize_size(void)
{
   return 0;
}

bool retro_serialize(void *data_, size_t size)
{
   return false;
}

bool retro_unserialize(const void *data_, size_t size)
{
   return false;
}

void *retro_get_memory_data(unsigned id)
{
   (void)id;
   return NULL;
}

size_t retro_get_memory_size(unsigned id)
{
   (void)id;
   return 0;
}

void retro_cheat_reset(void)
{}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
   (void)index;
   (void)enabled;
   (void)code;
}

