#include "settings.h"

const char KeyAssignment::ScancodeName[][16] = {
    "None",
    "Escape", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
    "PrintScreen", "ScrollLock", "Pause",
    "Insert", "Delete", "Home", "End", "PageUp", "PageDown",
    "Up", "Down", "Left", "Right",
    
    "~", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "Backspace",
    "[", "]", "\\", ";", "'", ",", ".", "/",
    "Tab", "CapsLock", "Return", "L Shift", "R Shift", "L Control", "R Control", "L Super", "R Super", "L Alt", "R Alt", "Spacebar", "Menu",
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
    "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
    "NumLock", "Keypad /", "Keypad *", "Keypad -", "Keypad +", "Keypad Enter", "Keypad .",      
    "Keypad 1", "Keypad 2", "Keypad 3", "Keypad 4", "Keypad 5", "Keypad 6", "Keypad 7", "Keypad 8", "Keypad 9", "Keypad 0"  
};

const KeyAssignment KeyAssignment::None(KeyAssignment::NONE, 0, 0);

const int Settings::Audio::FREQUENCIES[] = { 22500, 44100, 48000, 96000 };

nall::string KeyAssignment::name()
{
    if(type == KEYBOARD)
    {
        if(button > (unsigned)Keyboard::Scancode::Limit)
            button = (unsigned)Keyboard::Scancode::None;
        return ScancodeName[button];
    }
    
    if(type == JOYSTICK_AXIS)
    {
        unsigned axis = button >> 1;
        const char* dir = (button & 1) ? "+" : "-";
        if(axis == 0)
            return {"Joy ", joystick+1, " X-Axis", dir};
        else if(axis == 1)
            return {"Joy ", joystick+1, " Y-Axis", dir};
        else
            return {"Joy ", joystick+1, " Axis", axis+1, dir};
    }

    if(type == JOYSTICK_BUTTON)
        return {"Joy ", joystick+1, " Button ", button+1};

    return "None";
}



void Settings::appendKey(KeyAssignment& k, string name)
{
    append(k.type, string{name, ".type"});
    append(k.button, string{name, ".button"});
    append(k.joystick, string{name, ".joystick"});
}

Settings::Settings() : num_mice(0), pause(false), throttle(true), fullscreen(false), no_gui(false)
{
    append(audio.frequency = Audio::Frequency::FREQ_48000, "audio.frequency");
    append(audio.volume = 500, "audio.volume");
    append(audio.mute = false, "audio.mute");

    append(video.keep_aspect = true, "video.keep_aspect");
    
    // Paddle 1
    append(input.paddle[0].use_mouse = true, "input.paddle1.use_mouse");
    append(input.paddle[0].x_axis.mouse = 0, "input.paddle1.x_axis.mouse");
    append(input.paddle[0].x_axis.axis = 0, "input.paddle1.x_axis.axis");
    append(input.paddle[0].y_axis.mouse = 0, "input.paddle1.y_axis.mouse");
    append(input.paddle[0].y_axis.axis = 1, "input.paddle1.y_axis.axis");
    append(input.paddle[0].mouse_sensitivity = 500, "input.paddle1.mouse_sensitivity");

    append(input.paddle[0].use_keyboard = true, "input.paddle1.use_keyboard");
    appendKey(input.paddle[0].up = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::Up), "input.paddle1.up");
    appendKey(input.paddle[0].down = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::Down), "input.paddle1.down");
    appendKey(input.paddle[0].left = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::Left), "input.paddle1.left");
    appendKey(input.paddle[0].right = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::Right), "input.paddle1.right");
    append(input.paddle[0].keyboard_sensitivity = 250, "input.paddle1.keyboard_sensitivity");

    append(input.paddle[0].use_joystick = false,       "input.paddle1.use_joystick");
    append(input.paddle[0].joy_x_axis.joystick = 0,    "input.paddle1.joy_x_axis.joystick");
    append(input.paddle[0].joy_x_axis.axis = 0,        "input.paddle1.joy_x_axis.axis");
    append(input.paddle[0].joy_y_axis.joystick = 0,    "input.paddle1.joy_y_axis.joystick");
    append(input.paddle[0].joy_y_axis.axis = 1,        "input.paddle1.joy_y_axis.axis");
    append(input.paddle[0].joystick_mode = 0,          "input.paddle1.joystick_mode");
    append(input.paddle[0].joystick_sensitivity = 500, "input.paddle1.joystick_sensitivity");

    // Paddle 2
    append(input.paddle[1].use_mouse = false, "input.paddle2.use_mouse");
    append(input.paddle[1].x_axis.mouse = 0, "input.paddle2.x_axis.mouse");
    append(input.paddle[1].x_axis.axis = 0, "input.paddle2.x_axis.axis");
    append(input.paddle[1].y_axis.mouse = 0, "input.paddle2.y_axis.mouse");
    append(input.paddle[1].y_axis.axis = 1, "input.paddle2.y_axis.axis");
    append(input.paddle[1].mouse_sensitivity = 500, "input.paddle2.mouse_sensitivity");

    append(input.paddle[1].use_keyboard = true, "input.paddle2.use_keyboard");
    appendKey(input.paddle[1].up = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::W), "input.paddle2.up");
    appendKey(input.paddle[1].down = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::S), "input.paddle2.down");
    appendKey(input.paddle[1].left = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::A), "input.paddle2.left");
    appendKey(input.paddle[1].right = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::D), "input.paddle2.right");
    append(input.paddle[1].keyboard_sensitivity = 250, "input.paddle2.keyboard_sensitivity");

    append(input.paddle[1].use_joystick = false,       "input.paddle2.use_joystick");
    append(input.paddle[1].joy_x_axis.joystick = 0,    "input.paddle2.joy_x_axis.joystick");
    append(input.paddle[1].joy_x_axis.axis = 0,        "input.paddle2.joy_x_axis.axis");
    append(input.paddle[1].joy_y_axis.joystick = 0,    "input.paddle2.joy_y_axis.joystick");
    append(input.paddle[1].joy_y_axis.axis = 1,        "input.paddle2.joy_y_axis.axis");
    append(input.paddle[1].joystick_mode = 0,          "input.paddle2.joystick_mode");
    append(input.paddle[1].joystick_sensitivity = 500, "input.paddle2.joystick_sensitivity");

    // Paddle 3
    append(input.paddle[2].use_mouse = false, "input.paddle3.use_mouse");
    append(input.paddle[2].x_axis.mouse = 0, "input.paddle3.x_axis.mouse");
    append(input.paddle[2].x_axis.axis = 0, "input.paddle3.x_axis.axis");
    append(input.paddle[2].y_axis.mouse = 0, "input.paddle3.y_axis.mouse");
    append(input.paddle[2].y_axis.axis = 1, "input.paddle3.y_axis.axis");
    append(input.paddle[2].mouse_sensitivity = 500, "input.paddle3.mouse_sensitivity");

    append(input.paddle[2].use_keyboard = true, "input.paddle3.use_keyboard");
    appendKey(input.paddle[2].up = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::I), "input.paddle3.up");
    appendKey(input.paddle[2].down = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::K), "input.paddle3.down");
    appendKey(input.paddle[2].left = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::J), "input.paddle3.left");
    appendKey(input.paddle[2].right = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::L), "input.paddle3.right");
    append(input.paddle[2].keyboard_sensitivity = 250, "input.paddle3.keyboard_sensitivity");

    append(input.paddle[2].use_joystick = false,       "input.paddle3.use_joystick");
    append(input.paddle[2].joy_x_axis.joystick = 0,    "input.paddle3.joy_x_axis.joystick");
    append(input.paddle[2].joy_x_axis.axis = 0,        "input.paddle3.joy_x_axis.axis");
    append(input.paddle[2].joy_y_axis.joystick = 0,    "input.paddle3.joy_y_axis.joystick");
    append(input.paddle[2].joy_y_axis.axis = 1,        "input.paddle3.joy_y_axis.axis");
    append(input.paddle[2].joystick_mode = 0,          "input.paddle3.joystick_mode");
    append(input.paddle[2].joystick_sensitivity = 500, "input.paddle3.joystick_sensitivity");

    // Paddle 4
    append(input.paddle[3].use_mouse = false, "input.paddle4.use_mouse");
    append(input.paddle[3].x_axis.mouse = 0, "input.paddle4.x_axis.mouse");
    append(input.paddle[3].x_axis.axis = 0, "input.paddle4.x_axis.axis");
    append(input.paddle[3].y_axis.mouse = 0, "input.paddle4.y_axis.mouse");
    append(input.paddle[3].y_axis.axis = 1, "input.paddle4.y_axis.axis");
    append(input.paddle[3].mouse_sensitivity = 500, "input.paddle4.mouse_sensitivity");

    append(input.paddle[3].use_keyboard = true, "input.paddle4.use_keyboard");
    appendKey(input.paddle[3].up = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::T), "input.paddle4.up");
    appendKey(input.paddle[3].down = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::G), "input.paddle4.down");
    appendKey(input.paddle[3].left = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::F), "input.paddle4.left");
    appendKey(input.paddle[3].right = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::H), "input.paddle4.right");
    append(input.paddle[3].keyboard_sensitivity = 250, "input.paddle4.keyboard_sensitivity");

    append(input.paddle[3].use_joystick = false,       "input.paddle4.use_joystick");
    append(input.paddle[3].joy_x_axis.joystick = 0,    "input.paddle4.joy_x_axis.joystick");
    append(input.paddle[3].joy_x_axis.axis = 0,        "input.paddle4.joy_x_axis.axis");
    append(input.paddle[3].joy_y_axis.joystick = 0,    "input.paddle4.joy_y_axis.joystick");
    append(input.paddle[3].joy_y_axis.axis = 1,        "input.paddle4.joy_y_axis.axis");
    append(input.paddle[3].joystick_mode = 0,          "input.paddle4.joystick_mode");
    append(input.paddle[3].joystick_sensitivity = 500, "input.paddle4.joystick_sensitivity");

    // Throttle 1
    appendKey(input.throttle[0].key = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::Up), "input.throttle1.key");
    append(input.throttle[0].keyboard_sensitivity = 250, "input.throttle1.keyboard_sensitivity");

    // Joystick 1
    appendKey(input.joystick1.up = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::Up), "input.joystick1.up");
    appendKey(input.joystick1.down = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::Down), "input.joystick1.down");
    appendKey(input.joystick1.left = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::Left), "input.joystick1.left");
    appendKey(input.joystick1.right = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::Right), "input.joystick1.right");

    // Joystick 2
    appendKey(input.joystick2.up = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::W), "input.joystick2.up");
    appendKey(input.joystick2.down = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::S), "input.joystick2.down");
    appendKey(input.joystick2.left = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::A), "input.joystick2.left");
    appendKey(input.joystick2.right = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::D), "input.joystick2.right");

    // Wheel 1
    append(input.wheel[0].use_mouse = true, "input.wheel1.use_mouse");
    append(input.wheel[0].axis.mouse = 0, "input.wheel1.axis.mouse");
    append(input.wheel[0].axis.axis = 0, "input.wheel1.axis.axis");
    append(input.wheel[0].mouse_sensitivity = 500, "input.wheel1.mouse_sensitivity");

    append(input.wheel[0].use_keyboard = true, "input.wheel1.use_keyboard");
    appendKey(input.wheel[0].left = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::Left), "input.wheel1.left");
    appendKey(input.wheel[0].right = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::Right), "input.wheel1.right");
    append(input.wheel[0].keyboard_sensitivity = 500, "input.wheel1.keyboard_sensitivity");

    // Wheel 2
    append(input.wheel[1].use_mouse = false, "input.wheel2.use_mouse");
    append(input.wheel[1].axis.mouse = 0, "input.wheel2.axis.mouse");
    append(input.wheel[1].axis.axis = 0, "input.wheel2.axis.axis");
    append(input.wheel[1].mouse_sensitivity = 500, "input.wheel2.mouse_sensitivity");

    append(input.wheel[1].use_keyboard = true, "input.wheel2.use_keyboard");
    appendKey(input.wheel[1].left = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::A), "input.wheel2.left");
    appendKey(input.wheel[1].right = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::D), "input.wheel2.right");
    append(input.wheel[1].keyboard_sensitivity = 500, "input.wheel2.keyboard_sensitivity");

    // Buttons
    appendKey(input.buttons1.button1 = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::ControlLeft), "input.player1.button1");
    appendKey(input.buttons1.button2 = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::AltLeft), "input.player1.button2");
    appendKey(input.buttons1.button3 = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::Space), "input.player1.button3");

    appendKey(input.buttons2.button1 = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::J), "input.player2.button1");
    appendKey(input.buttons2.button2 = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::K), "input.player2.button2");
    appendKey(input.buttons2.button3 = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::L), "input.player2.button3");

    appendKey(input.coin_start.coin = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::Number5), "input.coin");
    appendKey(input.coin_start.start1 = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::Number1), "input.start1");
    appendKey(input.coin_start.start2 = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::Number2), "input.start2");

    // User Interface
    appendKey(input.ui.pause = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::P), "input.ui.pause");
    appendKey(input.ui.throttle = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::Insert), "input.ui.throttle");
    appendKey(input.ui.fullscreen = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::F11), "input.ui.fullscreen");
    appendKey(input.ui.quit = KeyAssignment(KeyAssignment::KEYBOARD, Keyboard::Scancode::Escape), "input.ui.quit");
} 

