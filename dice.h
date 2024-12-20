#ifndef DICE_H__
#define DICE_H__

#include <vector>

#include "circuit.h"
#include "circuit_desc.h"

using std::vector;
/*
 using phoenix::VerticalLayout;
using phoenix::Viewport; */

namespace dice_libretro {

#define VIDEO_WIDTH 640 // TODO (mittonk): Draw from dice.
#define VIDEO_HEIGHT 480
#define VIDEO_BYTES_PER_PIXEL 4
#define VIDEO_PIXELS VIDEO_WIDTH * VIDEO_HEIGHT
#define VIDEO_PITCH VIDEO_WIDTH * VIDEO_BYTES_PER_PIXEL

#define NUM_CONTROLLERS 1  // TODO (mittonk)

// RetroArch mask_id is a bit position, so do some shifting.
inline bool input_mask(int32_t input_bitmask, unsigned mask_id)
{
   return bool(input_bitmask & (1 << mask_id));
}


// Encapsulate the DICE emulator, for use in the libretro core.
class DICE
{
   private:
    Settings settings;
    Input* input;
    Video* video;
    Circuit* circuit;
    RealTimeClock real_time;

    //VerticalLayout layout;
    //Viewport* viewport;

   public:
      void init_mem(void);
      void update_input(int32_t *input_state);
      void run(void);
      void render_frame(void);
      void reset(void);
};


} // namespace
#endif
