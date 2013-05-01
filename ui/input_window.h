struct InputWindow : Window
{
    struct AssignmentSelector : HorizontalLayout
    {
        Button button;
        LineEdit line_edit;
        string name;

        InputWindow& window;
        KeyAssignment& key_assignment;
        
        AssignmentSelector(InputWindow& w, KeyAssignment& k, const string& n, unsigned button_width, unsigned edit_width) 
            : window(w), key_assignment(k), name(n)
        {
            button.setText({name, ":"});
            append(button, {button_width, 0}, 10);
            line_edit.setEditable(false);
            append(line_edit, {edit_width, 0});

            button.onActivate = [&]
            {
                if(window.active_selector)
                    window.active_selector->assign(KeyAssignment::None);
                
                window.active_selector = this;

                window.setEnabled(false);
                line_edit.setEditable(true);
                window.setStatusText({"Press a key to assign to '", name, "', or press 'Clear' to remove assignment"});
                SDL_JoystickEventState(SDL_ENABLE);
            };
        }

        void create()
        {
            line_edit.setText(key_assignment.name());
        }

        void assign(const KeyAssignment& k)
        {
            window.active_selector = nullptr;

            key_assignment = k;
            line_edit.setEditable(false);
            line_edit.setText(key_assignment.name());
            window.setEnabled(true);
            
            window.setStatusText("");
            SDL_JoystickEventState(SDL_DISABLE);
        }
    };
    
    struct PaddleLayout : HorizontalLayout
    {
        Settings::Input::Paddle& paddle_settings;

        // Mouse ///////////////////////////////////
        VerticalLayout mouse_layout;
        CheckBox mouse_box;
        HorizontalLayout axis_layout[2];
        ComboBox axis_box[2];
        Label axis_label[2];

        HorizontalLayout mouse_sensitivity_layout;
        Label mouse_sensitivity_label;
        LineEdit mouse_sensitivity_val;
        HorizontalSlider mouse_sensitivity_slider;

        Widget spacer;

        // Keyboard / Joystick ///////////////////////////////////
        VerticalLayout keyboard_layout;
        CheckBox keyboard_box;
        AssignmentSelector up_assignment, down_assignment, left_assignment, right_assignment;

        HorizontalLayout keyboard_sensitivity_layout;
        Label keyboard_sensitivity_label;
        LineEdit keyboard_sensitivity_val;
        HorizontalSlider keyboard_sensitivity_slider;

        void setEnabled(bool e = true)
        {
            mouse_box.setEnabled(e);
            axis_box[0].setEnabled(e && mouse_box.checked());
            axis_box[1].setEnabled(e && mouse_box.checked());
            mouse_sensitivity_slider.setEnabled(e && mouse_box.checked());
            
            keyboard_box.setEnabled(e);
            up_assignment.button.setEnabled(e && keyboard_box.checked());
            down_assignment.button.setEnabled(e && keyboard_box.checked());
            left_assignment.button.setEnabled(e && keyboard_box.checked());
            right_assignment.button.setEnabled(e && keyboard_box.checked());
            keyboard_sensitivity_slider.setEnabled(e && keyboard_box.checked());
        }

        void create()
        {
            mouse_box.setChecked(paddle_settings.use_mouse);
            mouse_box.onToggle();

            keyboard_box.setChecked(paddle_settings.use_keyboard);
            keyboard_box.onToggle();
            
            mouse_sensitivity_slider.setPosition(paddle_settings.mouse_sensitivity);
            mouse_sensitivity_slider.onChange();

            keyboard_sensitivity_slider.setPosition(paddle_settings.keyboard_sensitivity);
            keyboard_sensitivity_slider.onChange();

            up_assignment.create();
            down_assignment.create();
            left_assignment.create();
            right_assignment.create();
            
            axis_box[0].reset();
            axis_box[1].reset();
            for(unsigned i = 0; i < up_assignment.window.settings.num_mice; i++)
            {
                axis_box[0].append(string{"Mouse ", i+1, " X-Axis"}, string{"Mouse ", i+1, " Y-Axis"});
                axis_box[1].append(string{"Mouse ", i+1, " X-Axis"}, string{"Mouse ", i+1, " Y-Axis"});
            }
            axis_box[0].setSelection((paddle_settings.x_axis.mouse << 1) | paddle_settings.x_axis.axis);
            axis_box[1].setSelection((paddle_settings.y_axis.mouse << 1) | paddle_settings.y_axis.axis);
        }

        PaddleLayout(InputWindow& w, Settings::Input::Paddle& p) : 
            paddle_settings(p),
            up_assignment(w, p.up, "Up", 70, 100),
            down_assignment(w, p.down, "Down", 70, 100),
            left_assignment(w, p.left, "Left", 70, 100),
            right_assignment(w, p.right, "Right", 70, 100)
        {
            // Create mouse config
            mouse_box.setText("Use Mouse:");
            mouse_layout.append(mouse_box, { 0, 0 }, 10);

            axis_label[0].setText("Horizontal:");
            axis_label[1].setText("Vertical:");
            axis_box[0].onChange = [&]
            {
                paddle_settings.x_axis.mouse = axis_box[0].selection() >> 1;
                paddle_settings.x_axis.axis = axis_box[0].selection() & 1;
            };
            axis_box[1].onChange = [&]
            {
                paddle_settings.y_axis.mouse = axis_box[1].selection() >> 1;
                paddle_settings.y_axis.axis = axis_box[1].selection() & 1;
            };

            for(int x : {0, 1})
            {
                axis_layout[x].append(axis_label[x], {60, 0}, 5);
                axis_layout[x].append(axis_box[x], {0, 0});
                mouse_layout.append(axis_layout[x], {0, 0,}, 10);
            }

            mouse_sensitivity_label.setText("Mouse Sensitivity:");
            mouse_sensitivity_layout.append(mouse_sensitivity_label, {0, 0}, 10);
           
            mouse_sensitivity_val.setEditable(false);
            mouse_sensitivity_layout.append(mouse_sensitivity_val, {~0, 0});

            mouse_sensitivity_slider.setLength(1001);
            mouse_sensitivity_slider.onChange = [&]
            { 
                paddle_settings.mouse_sensitivity = mouse_sensitivity_slider.position();
                mouse_sensitivity_val.setText(mouse_sensitivity_slider.position() / 1000.0);
            };
               
            mouse_layout.append(mouse_sensitivity_layout, {~0, 0}, 10);
            mouse_layout.append(mouse_sensitivity_slider, {~0, 30});

            mouse_box.onToggle = [&]
            {
                paddle_settings.use_mouse = mouse_box.checked();
                axis_box[0].setEnabled(mouse_box.checked());
                axis_box[1].setEnabled(mouse_box.checked());
                mouse_sensitivity_slider.setEnabled(mouse_box.checked());
            };
            
            // Create keyboard/joystick config
            keyboard_box.setText("Use Keyboard / Joystick:");
            keyboard_layout.append(keyboard_box, { 0, 0 }, 10);

            keyboard_layout.append(up_assignment, {0, 0}, 10);
            keyboard_layout.append(down_assignment, {0, 0}, 10);
            keyboard_layout.append(left_assignment, {0, 0}, 10);
            keyboard_layout.append(right_assignment, {0, 0}, 10);

            keyboard_sensitivity_label.setText("Key / Joy Sensitivity:");
            keyboard_sensitivity_layout.append(keyboard_sensitivity_label, {0, 0}, 10);
           
            keyboard_sensitivity_val.setEditable(false);
            keyboard_sensitivity_layout.append(keyboard_sensitivity_val, {~0, 0});

            keyboard_sensitivity_slider.setLength(1001);
            keyboard_sensitivity_slider.onChange = [&]
            { 
                paddle_settings.keyboard_sensitivity = keyboard_sensitivity_slider.position();
                keyboard_sensitivity_val.setText(keyboard_sensitivity_slider.position() / 1000.0);
            };
              
            keyboard_layout.append(keyboard_sensitivity_layout, {~0, 0}, 10);
            keyboard_layout.append(keyboard_sensitivity_slider, {~0, 30});
            
            keyboard_box.onToggle = [&]
            {
                paddle_settings.use_keyboard = keyboard_box.checked();
                left_assignment.button.setEnabled(keyboard_box.checked());
                right_assignment.button.setEnabled(keyboard_box.checked());
                up_assignment.button.setEnabled(keyboard_box.checked());
                down_assignment.button.setEnabled(keyboard_box.checked());
                keyboard_sensitivity_slider.setEnabled(keyboard_box.checked());
            };

            append(mouse_layout, {0, ~0});
            append(spacer, {~0, ~0});
            append(keyboard_layout, {0, ~0});
        }
    };

    struct JoystickLayout : VerticalLayout
    {
        // Keyboard / Joystick ///////////////////////////////////
        AssignmentSelector up_assignment, down_assignment, left_assignment, right_assignment;

        JoystickLayout(InputWindow& w, Settings::Input::Joystick& j) : 
            up_assignment(w, j.up, "Up", 70, 100),
            down_assignment(w, j.down, "Down", 70, 100),
            left_assignment(w, j.left, "Left", 70, 100),
            right_assignment(w, j.right, "Right", 70, 100)
        {
            append(up_assignment, {0, 0}, 10);
            append(down_assignment, {0, 0}, 10);
            append(left_assignment, {0, 0}, 10);
            append(right_assignment, {0, 0}, 10);
        }

        void create()
        {
            up_assignment.create();
            down_assignment.create();
            left_assignment.create();
            right_assignment.create();
        }

        void setEnabled(bool e = true)
        {
            up_assignment.button.setEnabled(e);
            down_assignment.button.setEnabled(e);
            left_assignment.button.setEnabled(e);
            right_assignment.button.setEnabled(e);
        }
    };

    struct ButtonLayout : VerticalLayout
    {
        // Keyboard / Joystick ///////////////////////////////////
        AssignmentSelector button_1, button_2, button_3;

        ButtonLayout(InputWindow& w, Settings::Input::Button& b) : 
            button_1(w, b.button1, "Button 1", 80, 100),
            button_2(w, b.button2, "Button 2", 80, 100),
            button_3(w, b.button3, "Button 3", 80, 100)
        {
            append(button_1, {0, 0}, 10);
            append(button_2, {0, 0}, 10);
            append(button_3, {0, 0}, 10);
        }

        void create()
        {
            button_1.create();
            button_2.create();
            button_3.create();
        }

        void setEnabled(bool e = true)
        {
            button_1.button.setEnabled(e);
            button_2.button.setEnabled(e);
            button_3.button.setEnabled(e);
        }
    };

    struct CoinStartLayout : VerticalLayout
    {
        // Keyboard / Joystick ///////////////////////////////////
        AssignmentSelector coin, start1, start2;

        CoinStartLayout(InputWindow& w, Settings::Input::CoinStart& c) : 
            start1(w, c.start1, "Start Button 1", 100, 100),
            start2(w, c.start2, "Start Button 2", 100, 100),
            coin(w, c.coin, "Insert Coin", 100, 100)            
        {
            append(start1, {0, 0}, 10);
            append(start2, {0, 0}, 10);
            append(coin, {0, 0}, 10);            
        }

        void create()
        {
            start1.create();
            start2.create();
            coin.create();
        }

        void setEnabled(bool e = true)
        {
            start1.button.setEnabled(e);
            start2.button.setEnabled(e);
            coin.button.setEnabled(e);
        }
    };

    struct ThrottleLayout : VerticalLayout
    {
        // Keyboard / Joystick ///////////////////////////////////
        AssignmentSelector throttle_assignment;
        unsigned& keyboard_sensitivity;

        HorizontalLayout keyboard_sensitivity_layout;
        Label keyboard_sensitivity_label;
        LineEdit keyboard_sensitivity_val;
        HorizontalSlider keyboard_sensitivity_slider;

        ThrottleLayout(InputWindow& w, Settings::Input::Throttle& t) : 
            throttle_assignment(w, t.key, "Throttle", 70, 100),
            keyboard_sensitivity(t.keyboard_sensitivity)
        {
            append(throttle_assignment, {0, 0}, 10);

            keyboard_sensitivity_label.setText("Keyboard Sensitivity:");
            keyboard_sensitivity_layout.append(keyboard_sensitivity_label, {0, 0}, 10);
           
            keyboard_sensitivity_val.setEditable(false);
            keyboard_sensitivity_layout.append(keyboard_sensitivity_val, {~0, 0});

            keyboard_sensitivity_slider.setLength(1001);
            keyboard_sensitivity_slider.onChange = [&]
            { 
                keyboard_sensitivity = keyboard_sensitivity_slider.position();
                keyboard_sensitivity_val.setText(keyboard_sensitivity_slider.position() / 1000.0);
            };
              
            append(keyboard_sensitivity_layout, {throttle_assignment.minimumGeometry().width, 0}, 10);
            append(keyboard_sensitivity_slider, {throttle_assignment.minimumGeometry().width, 30});
        }

        void create()
        {
            throttle_assignment.create();

            keyboard_sensitivity_slider.setPosition(keyboard_sensitivity);
            keyboard_sensitivity_slider.onChange();
        }

        void setEnabled(bool e = true)
        {
            throttle_assignment.button.setEnabled(e);
            keyboard_sensitivity_slider.setEnabled(e);
        }
    };

    struct UserInterfaceLayout : VerticalLayout
    {
        // Keyboard / Joystick ///////////////////////////////////
        AssignmentSelector pause, throttle, fullscreen, quit;

        UserInterfaceLayout(InputWindow& w, Settings::Input::UserInterface& ui) : 
            pause(w, ui.pause, "Pause", 120, 100),
            throttle(w, ui.throttle, "Toggle Throttle", 120, 100),
            fullscreen(w, ui.fullscreen, "Toggle Fullscreen", 120, 100),
            quit(w, ui.quit, "Exit DICE", 120, 100)
        {
            append(pause, {0, 0}, 10);
            append(throttle, {0, 0}, 10);
            append(fullscreen, {0, 0}, 10);
            append(quit, {0, 0}, 10);
        }

        void create()
        {
            pause.create();
            throttle.create();
            fullscreen.create();
            quit.create();
        }

        void setEnabled(bool e = true)
        {
            pause.button.setEnabled(e);
            throttle.button.setEnabled(e);
            fullscreen.button.setEnabled(e);
            quit.button.setEnabled(e);
        }
    };

    Settings& settings;

    HorizontalLayout layout;
    ListView input_view;
    
    PaddleLayout paddle_layout_1, paddle_layout_2, paddle_layout_3, paddle_layout_4;
    JoystickLayout joystick_layout_1, joystick_layout_2;
    ThrottleLayout throttle_layout_1;
    ButtonLayout button_layout_1, button_layout_2;
    CoinStartLayout coin_start_layout;
    UserInterfaceLayout ui_layout;

    Layout* input_layouts[11];

    FixedLayout exit_layout;
    Button exit_button;

    AssignmentSelector* active_selector;

    InputWindow(Settings& s) : settings(s), active_selector(nullptr),
        paddle_layout_1(*this, s.input.paddle[0]), paddle_layout_2(*this, s.input.paddle[1]),
        paddle_layout_3(*this, s.input.paddle[2]), paddle_layout_4(*this, s.input.paddle[3]),
        joystick_layout_1(*this, s.input.joystick1), joystick_layout_2(*this, s.input.joystick2),
        throttle_layout_1(*this, s.input.throttle[0]),
        button_layout_1(*this, s.input.buttons1), button_layout_2(*this, s.input.buttons2),
        coin_start_layout(*this, s.input.coin_start), ui_layout(*this, s.input.ui),
        input_layouts{&paddle_layout_1, &joystick_layout_1, &throttle_layout_1, &button_layout_1,
                      &paddle_layout_2, &joystick_layout_2, &button_layout_2, &paddle_layout_3,
                      &paddle_layout_4, &coin_start_layout, &ui_layout}
    {
        layout.setMargin(10);

        input_view.append("Player 1 Paddle");
        input_view.append("Player 1 Joystick");
        input_view.append("Player 1 Throttle");
        input_view.append("Player 1 Buttons");

        input_view.append("Player 2 Paddle");
        input_view.append("Player 2 Joystick");
        input_view.append("Player 2 Buttons");

        input_view.append("Player 3 Paddle");
        input_view.append("Player 4 Paddle");

        input_view.append("Coin / Start");
        input_view.append("User Interface");
        
        input_view.onChange = [&]
        {
            for(auto x : input_layouts)
                layout.remove(*x);
            
            layout.append(*input_layouts[input_view.selection()], {~0, ~0});
        };

        layout.append(input_view, { 110, ~0 }, 10);

        exit_button.setText("Exit");
        exit_layout.append(exit_button, {440, 290 - exit_button.minimumGeometry().height, 90, exit_button.minimumGeometry().height});
    }

    void setEnabled(bool e = true)
    {
        input_view.setEnabled(e);
        
        exit_button.setText(e ? "Exit" : "Clear");

        input_layouts[input_view.selection()]->setEnabled(e);
    }

    void create(const Position& pos)
    {
        setGeometry({pos.x, pos.y, 540, 300});
        setTitle("Input Configuration");
        setResizable(false);
        setModal(true);
        
        paddle_layout_1.create();
        joystick_layout_1.create();
        throttle_layout_1.create();
        button_layout_1.create();
        paddle_layout_2.create();
        joystick_layout_2.create();
        button_layout_2.create();
        paddle_layout_3.create();
        paddle_layout_4.create();
        coin_start_layout.create();
        ui_layout.create();

        input_view.setSelection(0);
        input_view.setSelected(true);
        input_view.onChange();

        append(layout);
        append(exit_layout);
        setStatusVisible();
        setVisible(true);

        input_view.setFocused();
    }
};
