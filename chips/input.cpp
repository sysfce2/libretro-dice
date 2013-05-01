#include "input.h"
#include "../circuit.h"
#include "../settings.h"

#include "../manymouse/manymouse.h"
#include <SDL/SDL.h>

static const double INPUT_POLL_RATE = 10.0e-3; // 10 ms poll rate

extern CUSTOM_LOGIC( clock );

CHIP_DESC( PADDLE1_HORIZONTAL_INPUT ) = 
{
	CUSTOM_CHIP_START(&clock)
        OUTPUT_DELAY_S( INPUT_POLL_RATE, INPUT_POLL_RATE )
        OUTPUT_PIN( i1 ),
    
    ChipDesc((&AnalogInputDesc<0, true>::analog_input))
        INPUT_PINS( i1 )
        OUTPUT_PIN( i3 ),

	CHIP_DESC_END
};

CHIP_DESC( PADDLE2_HORIZONTAL_INPUT ) = 
{
	CUSTOM_CHIP_START(&clock)
        OUTPUT_DELAY_S( INPUT_POLL_RATE, INPUT_POLL_RATE )
        OUTPUT_PIN( i1 ),
    
    ChipDesc(&AnalogInputDesc<1, true>::analog_input)
        INPUT_PINS( i1 )
        OUTPUT_PIN( i3 ),

	CHIP_DESC_END
};

CHIP_DESC( PADDLE1_VERTICAL_INPUT ) = 
{
	CUSTOM_CHIP_START(&clock)
        OUTPUT_DELAY_S( INPUT_POLL_RATE, INPUT_POLL_RATE )
        OUTPUT_PIN( i1 ),
    
    ChipDesc(&AnalogInputDesc<0, false>::analog_input)
        INPUT_PINS( i1 )
        OUTPUT_PIN( i3 ),

	CHIP_DESC_END
};

CHIP_DESC( PADDLE2_VERTICAL_INPUT ) = 
{
	CUSTOM_CHIP_START(&clock)
        OUTPUT_DELAY_S( INPUT_POLL_RATE, INPUT_POLL_RATE )
        OUTPUT_PIN( i1 ),

    ChipDesc(&AnalogInputDesc<1, false>::analog_input)
        INPUT_PINS( i1 )
        OUTPUT_PIN( i3 ),

	CHIP_DESC_END
};

CHIP_DESC( PADDLE3_VERTICAL_INPUT ) = 
{
	CUSTOM_CHIP_START(&clock)
        OUTPUT_DELAY_S( INPUT_POLL_RATE, INPUT_POLL_RATE )
        OUTPUT_PIN( i1 ),
    
    ChipDesc(&AnalogInputDesc<2, false>::analog_input)
        INPUT_PINS( i1 )
        OUTPUT_PIN( i3 ),

	CHIP_DESC_END
};

CHIP_DESC( PADDLE4_VERTICAL_INPUT ) = 
{
	CUSTOM_CHIP_START(&clock)
        OUTPUT_DELAY_S( INPUT_POLL_RATE, INPUT_POLL_RATE )
        OUTPUT_PIN( i1 ),

    ChipDesc(&AnalogInputDesc<3, false>::analog_input)
        INPUT_PINS( i1 )
        OUTPUT_PIN( i3 ),

	CHIP_DESC_END
};


static const double ANALOG_THRESHOLD = 0.125; // Joystick dead zone, TODO: make configurable?

template <unsigned PADDLE, bool HORIZONTAL>
void AnalogInputDesc<PADDLE, HORIZONTAL>::analog_input(Chip* chip, int mask)
{
    Circuit* circuit = chip->circuit;
    Settings::Input::Paddle& settings = circuit->settings.input.paddle[PADDLE];

    AnalogInputDesc<PADDLE, HORIZONTAL>* desc = (AnalogInputDesc<PADDLE, HORIZONTAL>*)chip->custom_data;
    
    double delta = 0.0;

    if(settings.use_mouse)
    {
        // Scale sensitivity by total paddle range
        double sensitivity = double(settings.mouse_sensitivity) * fabs(desc->max_val - desc->min_val) / 100000.0; 
        
        if(HORIZONTAL && settings.x_axis.axis == 0) // Horizontal, using mouse x-axis
            delta += circuit->input.getRelativeMouseX(settings.x_axis.mouse) * sensitivity;
        else if(HORIZONTAL) // Horizontal, using mouse y-axis
            delta += circuit->input.getRelativeMouseY(settings.x_axis.mouse) * sensitivity;
        else if(settings.y_axis.axis == 0) // Vertical, using mouse x-axis
            delta += circuit->input.getRelativeMouseX(settings.y_axis.mouse) * sensitivity;
        else // Vertical, using mouse y-axis
            delta += circuit->input.getRelativeMouseY(settings.y_axis.mouse) * sensitivity;
    }

    if(settings.use_keyboard && HORIZONTAL)
    {
        double dt = (INPUT_POLL_RATE / Circuit::timescale) / 1000000000.0;
        double sensitivity = double(settings.keyboard_sensitivity) * fabs(desc->max_val - desc->min_val) / 100000.0; 

        switch(settings.left.type)
        {
            case KeyAssignment::KEYBOARD:
                delta -= circuit->input.getKeyboardState(settings.left.button) * dt * sensitivity;
                break;
            case KeyAssignment::JOYSTICK_BUTTON:
                delta -= circuit->input.getJoystickButton(settings.left.joystick, settings.left.button) * dt * sensitivity;
                break;
            case KeyAssignment::JOYSTICK_AXIS:
            {
                double val = circuit->input.getJoystickAxis(settings.left.joystick, settings.left.button >> 1) / 32768.0;
                if(val > ANALOG_THRESHOLD && (settings.left.button & 1))
                    delta -= val * dt * sensitivity;
                else if(val < -ANALOG_THRESHOLD && !(settings.left.button & 1))
                    delta += val * dt * sensitivity;
                break;
            }
            default: break;
        }
        switch(settings.right.type)
        {
            case KeyAssignment::KEYBOARD:
                delta += circuit->input.getKeyboardState(settings.right.button) * dt * sensitivity;
                break;
            case KeyAssignment::JOYSTICK_BUTTON:
                delta += circuit->input.getJoystickButton(settings.right.joystick, settings.right.button) * dt * sensitivity;
                break;
            case KeyAssignment::JOYSTICK_AXIS:
            {
                double val = circuit->input.getJoystickAxis(settings.right.joystick, settings.right.button >> 1) / 32768.0;
                if(val > ANALOG_THRESHOLD && (settings.right.button & 1))
                    delta += val * dt * sensitivity;
                else if(val < -ANALOG_THRESHOLD && !(settings.right.button & 1))
                    delta -= val * dt * sensitivity;
                break;
            }
            default: break;
        }
    }
    else if(settings.use_keyboard) // vertical
    {
        double dt = (INPUT_POLL_RATE / Circuit::timescale) / 1000000000.0;
        double sensitivity = double(settings.keyboard_sensitivity) * fabs(desc->max_val - desc->min_val) / 100000.0; 

        switch(settings.down.type)
        {
            case KeyAssignment::KEYBOARD:
                delta += circuit->input.getKeyboardState(settings.down.button) * dt * sensitivity;
                break;
            case KeyAssignment::JOYSTICK_BUTTON:
                delta += circuit->input.getJoystickButton(settings.down.joystick, settings.down.button) * dt *sensitivity;
                break;
            case KeyAssignment::JOYSTICK_AXIS:
            {
                double val = circuit->input.getJoystickAxis(settings.down.joystick, settings.down.button >> 1) / 32768.0;
                if(val > ANALOG_THRESHOLD && (settings.down.button & 1))
                    delta += val * dt * sensitivity;
                else if(val < -ANALOG_THRESHOLD && !(settings.down.button & 1))
                    delta -= val * dt * sensitivity;
                break;
            }
            default: break;
        }
        switch(settings.up.type)
        {
            case KeyAssignment::KEYBOARD:
                delta -= circuit->input.getKeyboardState(settings.up.button) * dt * sensitivity;
                break;
            case KeyAssignment::JOYSTICK_BUTTON:
                delta -= circuit->input.getJoystickButton(settings.up.joystick, settings.up.button) * dt * sensitivity;
                break;
            case KeyAssignment::JOYSTICK_AXIS:
            {
                double val = circuit->input.getJoystickAxis(settings.up.joystick, settings.up.button >> 1) / 32768.0;
                if(val > ANALOG_THRESHOLD && (settings.up.button & 1))
                    delta -= val * dt * sensitivity;
                else if(val < -ANALOG_THRESHOLD && !(settings.up.button & 1))
                    delta += val * dt * sensitivity;
                break;
            }
            default: break;
        }
    }

    double prev_val = desc->current_val;
    
    if(desc->max_val > desc->min_val)
    {
        desc->current_val += delta;
        if(desc->current_val > desc->max_val)      desc->current_val = desc->max_val;
        else if(desc->current_val < desc->min_val) desc->current_val = desc->min_val;
    }
    else // swap max,min values for reversed paddle direction
    {
        desc->current_val -= delta;
        if(desc->current_val < desc->max_val)      desc->current_val = desc->max_val;
        else if(desc->current_val > desc->min_val) desc->current_val = desc->min_val;
    }

    //if(desc->current_val != prev_val && desc->mono_555) // Update resistance value in 555
    {
        desc->mono_555->r = desc->current_val;
        
        //chip->deactivate_outputs(); // TODO: does this improve things?
        //chip->state = PASSIVE;
        //chip->active_outputs = (1 << chip->output_links.size()) - 1;
        
        chip->pending_event = chip->circuit->queue_push(chip, 0);
    }
}


// My sincerest apologies
template <typename C, C (Settings::Input::*c), KeyAssignment (C::*k)>
CUSTOM_LOGIC( digital_input )
{
    Circuit* circuit = chip->circuit;
    KeyAssignment& key_assignment = circuit->settings.input.*c.*k;

    int new_out = circuit->input.getKeyPressed(key_assignment);
    new_out ^= 1; // Joysticks, buttons are active LOW

    if(new_out != chip->output)
    {
        //chip->deactivate_outputs();
        //chip->state = PASSIVE;
        //chip->active_outputs = (1 << chip->output_links.size()) - 1;   

        // Generate output event
        chip->pending_event = chip->circuit->queue_push(chip, 0);
    }
}

template <int N> CHIP_LOGIC( button_inv )
{
    pin[i1 + N - 1] = pin[N] ^ 1;
}

CHIP_DESC( COIN_INPUT ) = 
{
	CUSTOM_CHIP_START(&clock)
        OUTPUT_DELAY_S( INPUT_POLL_RATE, INPUT_POLL_RATE )
        OUTPUT_PIN( i7 ),

    // Normally Open (Active Low) Output
    ChipDesc(&digital_input<Settings::Input::CoinStart, &Settings::Input::coin_start, &Settings::Input::CoinStart::coin>)
        INPUT_PINS( i7 )
        OUTPUT_PIN( 1 ),

    // Normally Closed (Active High) Outputs
    CHIP_START(button_inv<1>) INPUT_PINS( 1 ) OUTPUT_PIN( i1 ),

	CHIP_DESC_END
};

CHIP_DESC( START_INPUT ) = 
{
	CUSTOM_CHIP_START(&clock)
        OUTPUT_DELAY_S( INPUT_POLL_RATE, INPUT_POLL_RATE )
        OUTPUT_PIN( i7 ),

    // Normally Open (Active Low) Outputs
    ChipDesc(&digital_input<Settings::Input::CoinStart, &Settings::Input::coin_start, &Settings::Input::CoinStart::start1>)
        INPUT_PINS( i7 )
        OUTPUT_PIN( 1 ),

    ChipDesc(&digital_input<Settings::Input::CoinStart, &Settings::Input::coin_start, &Settings::Input::CoinStart::start2>)
        INPUT_PINS( i7 )
        OUTPUT_PIN( 2 ),

    // Normally Closed (Active High) Outputs
    CHIP_START(button_inv<1>) INPUT_PINS( 1 ) OUTPUT_PIN( i1 ),
    CHIP_START(button_inv<2>) INPUT_PINS( 2 ) OUTPUT_PIN( i2 ),

	CHIP_DESC_END
};

CHIP_DESC( JOYSTICK1_INPUT ) = 
{
	CUSTOM_CHIP_START(&clock)
        OUTPUT_DELAY_S( INPUT_POLL_RATE, INPUT_POLL_RATE )
        OUTPUT_PIN( i7 ),

    // Normally Open (Active Low) Outputs
    ChipDesc(&digital_input<Settings::Input::Joystick, &Settings::Input::joystick1, &Settings::Input::Joystick::up>)
        INPUT_PINS( i7 )
        OUTPUT_PIN( Joystick::UP ),

	ChipDesc(&digital_input<Settings::Input::Joystick, &Settings::Input::joystick1, &Settings::Input::Joystick::down>)
        INPUT_PINS( i7 )
        OUTPUT_PIN( Joystick::DOWN ),

	ChipDesc(&digital_input<Settings::Input::Joystick, &Settings::Input::joystick1, &Settings::Input::Joystick::left>)
        INPUT_PINS( i7 )
        OUTPUT_PIN( Joystick::LEFT ),

	ChipDesc(&digital_input<Settings::Input::Joystick, &Settings::Input::joystick1, &Settings::Input::Joystick::right>)
        INPUT_PINS( i7 )
        OUTPUT_PIN( Joystick::RIGHT ),

	CHIP_DESC_END
};

CHIP_DESC( JOYSTICK2_INPUT ) = 
{
	CUSTOM_CHIP_START(&clock)
        OUTPUT_DELAY_S( INPUT_POLL_RATE, INPUT_POLL_RATE )
        OUTPUT_PIN( i7 ),

    // Normally Open (Active Low) Outputs
    ChipDesc(&digital_input<Settings::Input::Joystick, &Settings::Input::joystick2, &Settings::Input::Joystick::up>)
        INPUT_PINS( i7 )
        OUTPUT_PIN( Joystick::UP ),

	ChipDesc(&digital_input<Settings::Input::Joystick, &Settings::Input::joystick2, &Settings::Input::Joystick::down>)
        INPUT_PINS( i7 )
        OUTPUT_PIN( Joystick::DOWN ),

	ChipDesc(&digital_input<Settings::Input::Joystick, &Settings::Input::joystick2, &Settings::Input::Joystick::left>)
        INPUT_PINS( i7 )
        OUTPUT_PIN( Joystick::LEFT ),

	ChipDesc(&digital_input<Settings::Input::Joystick, &Settings::Input::joystick2, &Settings::Input::Joystick::right>)
        INPUT_PINS( i7 )
        OUTPUT_PIN( Joystick::RIGHT ),

	CHIP_DESC_END
};

CHIP_DESC( BUTTONS1_INPUT ) = 
{
	CUSTOM_CHIP_START(&clock)
        OUTPUT_DELAY_S( INPUT_POLL_RATE, INPUT_POLL_RATE )
        OUTPUT_PIN( i7 ),

    // Normally Open (Active Low) Outputs
    ChipDesc(&digital_input<Settings::Input::Button, &Settings::Input::buttons1, &Settings::Input::Button::button1>)
        INPUT_PINS( i7 )
        OUTPUT_PIN( 1 ),

	ChipDesc(&digital_input<Settings::Input::Button, &Settings::Input::buttons1, &Settings::Input::Button::button2>)
        INPUT_PINS( i7 )
        OUTPUT_PIN( 2 ),

	ChipDesc(&digital_input<Settings::Input::Button, &Settings::Input::buttons1, &Settings::Input::Button::button3>)
        INPUT_PINS( i7 )
        OUTPUT_PIN( 3 ),

    // Normally Closed (Active High) Outputs
    CHIP_START(button_inv<1>) INPUT_PINS( 1 ) OUTPUT_PIN( i1 ),
    CHIP_START(button_inv<2>) INPUT_PINS( 2 ) OUTPUT_PIN( i2 ),
    CHIP_START(button_inv<3>) INPUT_PINS( 3 ) OUTPUT_PIN( i3 ),

	CHIP_DESC_END
};


CHIP_DESC( BUTTONS2_INPUT ) = 
{
	CUSTOM_CHIP_START(&clock)
        OUTPUT_DELAY_S( INPUT_POLL_RATE, INPUT_POLL_RATE )
        OUTPUT_PIN( i7 ),

    // Normally Open (Active Low) Outputs
    ChipDesc(&digital_input<Settings::Input::Button, &Settings::Input::buttons2, &Settings::Input::Button::button1>)
        INPUT_PINS( i7 )
        OUTPUT_PIN( 1 ),

	ChipDesc(&digital_input<Settings::Input::Button, &Settings::Input::buttons2, &Settings::Input::Button::button2>)
        INPUT_PINS( i7 )
        OUTPUT_PIN( 2 ),

	ChipDesc(&digital_input<Settings::Input::Button, &Settings::Input::buttons2, &Settings::Input::Button::button3>)
        INPUT_PINS( i7 )
        OUTPUT_PIN( 3 ),

    // Normally Closed (Active High) Outputs
    CHIP_START(button_inv<1>) INPUT_PINS( 1 ) OUTPUT_PIN( i1 ),
    CHIP_START(button_inv<2>) INPUT_PINS( 2 ) OUTPUT_PIN( i2 ),
    CHIP_START(button_inv<3>) INPUT_PINS( 3 ) OUTPUT_PIN( i3 ),

	CHIP_DESC_END
};


template <unsigned THROTTLE>
void ThrottleDesc<THROTTLE>::throttle_input(Chip* chip, int mask)
{
    Circuit* circuit = chip->circuit;
    Settings::Input::Throttle& settings = circuit->settings.input.throttle[THROTTLE];

    ThrottleDesc<THROTTLE>* desc = (ThrottleDesc<THROTTLE>*)chip->custom_data;
    
    double dt = (INPUT_POLL_RATE / Circuit::timescale) / 1.0e12;
    double* pos = desc->pos;

    switch(settings.key.type)
    {
        case KeyAssignment::KEYBOARD:
            if(circuit->input.getKeyboardState(settings.key.button))
                *pos += dt * double(settings.keyboard_sensitivity);
            else
                *pos -= dt * double(settings.keyboard_sensitivity);
            break;
        case KeyAssignment::JOYSTICK_BUTTON:
            if(circuit->input.getJoystickButton(settings.key.joystick, settings.key.button))
                *pos += dt * double(settings.keyboard_sensitivity);
            else
                *pos -= dt * double(settings.keyboard_sensitivity);
            break;
        case KeyAssignment::JOYSTICK_AXIS:
            if(settings.key.button & 1)
                *pos = circuit->input.getJoystickAxis(settings.key.joystick, settings.key.button >> 1) / 327.68;
            else 
                *pos = circuit->input.getJoystickAxis(settings.key.joystick, settings.key.button >> 1) / -327.68;
            break;
    }

    if(*pos < 0.0) *pos = 0.0;
    else if(*pos > 100.0) *pos = 100.0;

    chip->deactivate_outputs(); // TODO: does this improve things?
    //chip->state = PASSIVE;
    //chip->active_outputs = (1 << chip->output_links.size()) - 1;
        
    chip->pending_event = chip->circuit->queue_push(chip, 0);
}

CHIP_DESC( THROTTLE1_INPUT ) = 
{
	CUSTOM_CHIP_START(&clock)
        OUTPUT_DELAY_S( INPUT_POLL_RATE, INPUT_POLL_RATE )
        OUTPUT_PIN( i1 ),
    
    ChipDesc((&ThrottleDesc<0>::throttle_input))
        INPUT_PINS( i1 )
        OUTPUT_PIN( i3 ),

	CHIP_DESC_END
};






Input::Input()
{ }

Input::~Input()
{
    for(int i = 0; i < joysticks.size(); i++)
        if(joysticks[i]) SDL_JoystickClose(joysticks[i]);
}

void Input::init()
{
    joysticks.resize(SDL_NumJoysticks());

    for(int i = 0; i < joysticks.size(); i++)
        joysticks[i] = SDL_JoystickOpen(i);
        // TODO: check if joystick failed to open
}

void Input::poll_input()
{
    SDL_JoystickUpdate();

    OS::processEvents();

    ManyMouseEvent mouse_event;
    while (ManyMouse_PollEvent(&mouse_event))
    {
        int mouse = mouse_event.device;
        
        if(mouse >= mouse_rel_x.size())
        {
            mouse_rel_x.resize(mouse+1);
            mouse_rel_y.resize(mouse+1);
        }

        if (mouse_event.type == MANYMOUSE_EVENT_RELMOTION)
        {
            if(mouse_event.item == 0)
                mouse_rel_x[mouse] += mouse_event.value;
            else if (mouse_event.item == 1)
                mouse_rel_y[mouse] += mouse_event.value;
        }
        else if (mouse_event.type == MANYMOUSE_EVENT_ABSMOTION)
        {
            // TODO: Handle absolute motion?
            /*double val = (double) (mouse_event.value - mouse_event.minval);
            double maxval = (double) (mouse_event.maxval - mouse_event.minval);
            if (mouse_event.item == 0)
                mouse->x = (val / maxval);
            else if (mouse_event.item == 1)
                mouse->y = (val / maxval);*/
        }
    }
}

int Input::getRelativeMouseX(unsigned mouse)
{
    if(mouse >= mouse_rel_x.size())
    {
        mouse_rel_x.resize(mouse+1);
        mouse_rel_y.resize(mouse+1);
    }

    int x = mouse_rel_x[mouse];
    mouse_rel_x[mouse] = 0;
    return x;
}

int Input::getRelativeMouseY(unsigned mouse)
{
    if(mouse >= mouse_rel_y.size())
    {
        mouse_rel_x.resize(mouse+1);
        mouse_rel_y.resize(mouse+1);
    }

    int y = mouse_rel_y[mouse];
    mouse_rel_y[mouse] = 0;
    return y;
}

bool Input::getKeyboardState(unsigned scancode)
{
    return Keyboard::pressed((Keyboard::Scancode)scancode);
}

bool Input::getJoystickButton(unsigned joystick, unsigned button)
{
    return SDL_JoystickGetButton(joysticks[joystick], button);
}

int16_t Input::getJoystickAxis(unsigned joystick, unsigned axis)
{
    return SDL_JoystickGetAxis(joysticks[joystick], axis);
}

bool Input::getKeyPressed(const KeyAssignment& key_assignment)
{
    switch(key_assignment.type)
    {
        case KeyAssignment::KEYBOARD:
            return getKeyboardState(key_assignment.button);
        case KeyAssignment::JOYSTICK_BUTTON:
            return getJoystickButton(key_assignment.joystick, key_assignment.button);
        case KeyAssignment::JOYSTICK_AXIS:
        {
            int16_t val = getJoystickAxis(key_assignment.joystick, key_assignment.button >> 1);
            return (key_assignment.button & 1) ? (val > Joystick::BUTTON_THRESHOLD) : (val < -Joystick::BUTTON_THRESHOLD);
        }
        default: return false;
    }
}
