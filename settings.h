#ifndef SETTINGS_H
#define SETTINGS_H

#include <phoenix.hpp>

using namespace nall;
using namespace phoenix;

struct KeyAssignment
{
    enum InputType { NONE = 0, KEYBOARD, JOYSTICK_AXIS, JOYSTICK_BUTTON };

    unsigned type;
    unsigned button;
    unsigned joystick;

    KeyAssignment(unsigned t = NONE, unsigned b = 0, unsigned j = 0) :
        type(t), button(b), joystick(j) { }

    KeyAssignment(unsigned t, Keyboard::Scancode s) :
        type(t), button((unsigned)s), joystick(0) { }

    nall::string name();

    static const char ScancodeName[][16];
    static const KeyAssignment None;
};

struct Settings : configuration
{
    nall::string filename;
    unsigned num_mice;

    bool pause, throttle;
    bool fullscreen;
    bool no_gui;
    
    struct Audio
    {
        enum Frequency { FREQ_22500 = 0, FREQ_44100, FREQ_48000, FREQ_96000 };
        static const int FREQUENCIES[];

        unsigned frequency;
        unsigned volume; // Fixed point, tenths of percent
        bool mute;

    } audio;

    struct Video
    {
        bool keep_aspect;
    } video;

    struct Input
    {
        struct Paddle
        {
            struct MouseAxis
            {
                unsigned mouse;
                unsigned axis;
            };
            bool use_mouse;
            MouseAxis x_axis, y_axis;
            unsigned mouse_sensitivity;

            bool use_keyboard;
            unsigned keyboard_sensitivity;
            KeyAssignment up, down, left, right;

            struct JoystickAxis
            {
                unsigned joystick;
                unsigned axis;
            };
            enum JoystickMode { JOYSTICK_RELATIVE = 0, JOYSTICK_ABSOLUTE = 1 };
            bool use_joystick;
            JoystickAxis joy_x_axis, joy_y_axis;
            unsigned joystick_mode;
            unsigned joystick_sensitivity;
        };

        struct Wheel
        {
            bool use_mouse;

            struct Axis
            {
                unsigned mouse;
                unsigned axis;
            };
            Axis axis;
            unsigned mouse_sensitivity;

            bool use_keyboard;
            unsigned keyboard_sensitivity;

            KeyAssignment left, right;
        };

        struct Throttle
        {
            KeyAssignment key;
            unsigned keyboard_sensitivity;
        };

        struct Joystick
        {
            KeyAssignment up, down, left, right;
        };

        struct Button
        {
            KeyAssignment button1, button2, button3;
        };

        struct CoinStart
        {
            KeyAssignment coin, start1, start2;
        };

        struct UserInterface
        {
            KeyAssignment pause, throttle, fullscreen, quit;
        };

        Paddle paddle[4];
        Throttle throttle[1];
        Joystick joystick1, joystick2;
        Wheel wheel[2];
        Button buttons1, buttons2;
        CoinStart coin_start;
        UserInterface ui;

    } input;

    Settings();

    void appendKey(KeyAssignment& k, string name);

    bool load()
    {
        configuration::load(filename);
        return save();  // Create file if it doesn't exist
    }

    bool save()
    {
        return configuration::save(filename);
    }
};

#endif
