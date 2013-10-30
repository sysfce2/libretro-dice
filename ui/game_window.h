#include "../chips/input.h"

struct GameWindow : Window
{
    VerticalLayout layout;
    HorizontalLayout inner_layout;
    
    ListView game_view;
    
    VerticalLayout control_layout;
    Label control_label;
    
    struct TextLayout : HorizontalLayout
    {
        Widget spacer;
        Label label;
        std::vector<TextLayout*> sub_layouts;

        TextLayout(unsigned font_size, int space, nall::string text)
        {
            if(font_size != 0) label.setFont({"Sans, ", font_size});
            label.setText(text);
            append(spacer, {space, 0});
            append(label, {0, 0});
        }

        void append_all(VerticalLayout& layout)
        {
            layout.append(*this, {0, 0}, 2);
            for(TextLayout* l : sub_layouts)
                l->append_all(layout);
        }

        void remove_all(VerticalLayout& layout)
        {
            for(TextLayout* l : sub_layouts)
                l->remove_all(layout);
            layout.remove(*this);
        }
    };

    std::vector<TextLayout*> game_layouts;

    HorizontalLayout button_layout;
    Button start_button;
    Button cancel_button;
    Label spacer;

    GameWindow()
    {
        setTitle("New Game");
        setResizable(false);
        
        layout.setMargin(10);
        game_view.setHeaderText("Game", "Manufacturer", "Year");
        game_view.setHeaderVisible();

        for(const GameDesc& g : game_list)
        {
            game_view.append(g.name, g.manufacturer, g.year);

            game_layouts.push_back(new TextLayout(11, 0, "Controls"));
            std::vector<TextLayout*>& player_layouts = game_layouts.back()->sub_layouts;
            player_layouts.push_back(new TextLayout(9, 20, "Coin / Start"));

            for(CircuitDesc* desc = g.desc; desc->type != CIRCUIT_END; desc++)
            {
                if(desc->type != INPUT_INST) continue;

                for(InputDesc* input = desc->u.input; input->chip != NULL; input++)
                {
                    int player = get_player(input->chip);
                    while(player >= player_layouts.size())
                        player_layouts.push_back(new TextLayout(9, 20, {"Player ", player_layouts.size()}));

                    std::vector<TextLayout*>& current_player = player_layouts[player]->sub_layouts;
                    current_player.push_back(new TextLayout(8, 40, {get_name(input->chip), 
                                                                    get_pin_name(input->chip, input->pins), 
                                                                    ": ", input->info}));
                }

                // Rotate Coin/Start to end
                TextLayout* t = player_layouts.front();
                player_layouts.erase(player_layouts.begin());
                player_layouts.push_back(t);
            }
        }

        game_view.autoSizeColumns();

        start_button.setText("Start Game");
        cancel_button.setText("Cancel");
        //cancel_button.onActivate = [&] { setVisible(false); };

        button_layout.append(start_button, {100, 0});
        button_layout.append(spacer, {~0, ~0});
        button_layout.append(cancel_button, {100, 0});

        inner_layout.append(game_view, {300, ~0}, 10);
        
        game_view.onChange = [&]
        {
            for(auto x : game_layouts)
                x->remove_all(control_layout);

            int current_config = game_view.selection();
            game_layouts[current_config]->append_all(control_layout);
        };

        inner_layout.append(control_layout, {~0, ~0}, 10);
        layout.append(inner_layout, {~0, ~0}, 10);
        layout.append(button_layout, {~0, 0});
        append(layout);

        game_view.onChange();
    }

    void create(const Position& pos)
    {
        setGeometry({pos.x, pos.y, 560, 350});

        game_view.setSelection(0);
        game_view.setSelected(true);
        //game_view.onChange();
        
        setVisible(true);
        game_view.setFocused();
        setModal(true);
    }

    static int get_player(ChipDesc* chip)
    {
        if(chip == chip_PADDLE1_HORIZONTAL_INPUT ||
           chip == chip_PADDLE1_VERTICAL_INPUT ||
           chip == chip_THROTTLE1_INPUT ||
           chip == chip_JOYSTICK1_INPUT ||
           chip == chip_WHEEL1_INPUT ||
           chip == chip_BUTTONS1_INPUT)
                return 1;
        if(chip == chip_PADDLE2_HORIZONTAL_INPUT ||
           chip == chip_PADDLE2_VERTICAL_INPUT ||
           chip == chip_JOYSTICK2_INPUT ||
           chip == chip_WHEEL2_INPUT ||
           chip == chip_BUTTONS2_INPUT)
                return 2;
        if(chip == chip_PADDLE3_HORIZONTAL_INPUT ||
           chip == chip_PADDLE3_VERTICAL_INPUT)
                return 3;
        if(chip == chip_PADDLE4_HORIZONTAL_INPUT ||
           chip == chip_PADDLE4_VERTICAL_INPUT)
                return 4;

        return 0;
    }

    static nall::string get_name(ChipDesc* chip)
    {
        if(chip == chip_PADDLE1_HORIZONTAL_INPUT ||
           chip == chip_PADDLE1_VERTICAL_INPUT ||
           chip == chip_PADDLE2_HORIZONTAL_INPUT ||
           chip == chip_PADDLE2_VERTICAL_INPUT ||
           chip == chip_PADDLE3_HORIZONTAL_INPUT ||
           chip == chip_PADDLE3_VERTICAL_INPUT ||
           chip == chip_PADDLE4_HORIZONTAL_INPUT ||
           chip == chip_PADDLE4_VERTICAL_INPUT)
                return "Paddle";
        if(chip == chip_THROTTLE1_INPUT)
                return "Throttle";
        if(chip == chip_JOYSTICK1_INPUT ||
           chip == chip_JOYSTICK2_INPUT)
                return "Joystick";
        if(chip == chip_WHEEL1_INPUT ||
           chip == chip_WHEEL2_INPUT)
                return "Wheel";
        if(chip == chip_BUTTONS1_INPUT ||
           chip == chip_BUTTONS2_INPUT)
                return "Button";
        if(chip == chip_COIN_INPUT)
                return "Coin";
        if(chip == chip_START_INPUT)
                return "Start";
        
        return "";
    }

    static nall::string get_pin_name(ChipDesc* chip, std::array<uint8_t, 4>& pins)
    {
        int count = 0;
        while(count < pins.size() && pins[count] != 0) count++;
        
        nall::string s = "";

        if(chip == chip_JOYSTICK1_INPUT || chip == chip_JOYSTICK2_INPUT)
        {
            if(count == 0) return "";
            s = " ";
            for(unsigned i = 0; i < count; i++)
            {
                switch(pins[i])
                {
                    case Joystick::UP: s.append("Up"); break;
                    case Joystick::DOWN: s.append("Down"); break;
                    case Joystick::LEFT: s.append("Left"); break;
                    case Joystick::RIGHT: s.append("Right"); break;
                }
                if(i != count-1) s.append(", ");
            }
        }
        else if(chip == chip_BUTTONS1_INPUT || chip == chip_BUTTONS2_INPUT)
        {
            if(count == 0) return "s";
            s = (count == 1) ? "" : "s";
            s.append(" ", pins[0]);
            for(unsigned i = 1; i < count; i++) s.append(", ", pins[i]);
        }
        else if(chip == chip_COIN_INPUT || chip == chip_START_INPUT)
        {
            if(count == 0) return "";
            s.append(" ", pins[0]);
            for(unsigned i = 1; i < count; i++) s.append(", ", pins[i]);
        }
        return s;
    }
};
