#include "../chips/dipswitch.h"

struct DipswitchWindow : Window
{
    struct DipswitchSelector : HorizontalLayout
    {
        Label name;
        RadioButton settings[2];
        unsigned& state;

        DipswitchSelector(const char* n, unsigned& sta, const char* set[2]) : state(sta)
        {
            name.setText({n, ":"});
            settings[0].setText(set[0]);
            settings[1].setText(set[1]);

            RadioButton::group(settings[0], settings[1]);
            settings[state].setChecked();

            settings[0].onActivate = settings[1].onActivate = [&]
            {
                state = settings[1].checked();
            };

            append(name, {0, 0}, 10);
            append(settings[0], {0, 0}, 10);
            append(settings[1], {0, 0});
        }
    };

    struct HexDipswitchSelector : HorizontalLayout
    {
        Label name;
        ComboButton settings;
        unsigned& state;

        HexDipswitchSelector(const char* n, unsigned& sta, const char* set[], int size = 16) : state(sta)
        {
            name.setText({n, ":"});
            for(int i = 0; i < size; i++) 
                if(set[i] != NULL) settings.append(set[i]);

            settings.setSelection(state);

            settings.onChange = [&]
            {
                state = settings.selection();
            };

            append(name, {0, 0}, 10);
            append(settings, {0, 0});
        }
    };

    struct PotentiometerSelector : HorizontalLayout
    {
        Label name;
        LineEdit edit;
        HorizontalSlider slider;
        double& val;
        double min;
        double max;

        PotentiometerSelector(const char* n, double& v, double mi, double ma) : val(v), min(mi), max(ma)
        {
            name.setText({n, ":"});

            edit.setEditable(false);

            slider.onChange = [&]
            {
                val = (max-min) * slider.position() / 1000.0 + min;
                edit.setText({slider.position() / 10.0, "%"});
            };

            slider.setLength(1001);
            slider.setPosition(1000.0 * (val - min) / (max-min));
            edit.setText({slider.position() / 10.0, "%"});

            append(name, {0, 0}, 10);
            append(edit, {50, 0}, 10);
            append(slider, {~0, ~0});
        }
    };


    HorizontalLayout layout;
    ListView game_view;

    std::vector<Layout*> game_layouts;
    std::vector<GameConfig> game_configs;
    int current_config;

    FixedLayout exit_layout;
    Button exit_button;

    DipswitchWindow() : current_config(0)
    {
        setTitle("DIP Switch Configuration");
        setResizable(false);
        
        layout.setMargin(10);

        // Search all circuit descriptors for DIP switches, add to layouts
        for(const GameDesc& g : game_list)
        {
            bool has_config = false;
            
            for(CircuitDesc* desc = g.desc; desc->type != CIRCUIT_END; desc++)
            {
                if(desc->u.instance.chip == chip_DIPSWITCH ||
                   desc->u.instance.chip == chip_DIPSWITCH_SP4T ||
                   desc->u.instance.chip == chip_53137 || 
                   desc->u.instance.chip == chip_DIPSWITCH_4SPST ||
                   desc->u.instance.chip == chip_POT_555_ASTABLE ||
                   desc->u.instance.chip == chip_POT_555_MONO)
                {
                    has_config = true;
                    break;
                }
            }

            if(!has_config) continue;

            // Load configuration from game config file
            game_configs.push_back(GameConfig(g.desc, g.command_line));

            game_view.append(g.name);
            
            VerticalLayout* game_layout = new VerticalLayout();
            game_layouts.push_back(game_layout);
            
            for(CircuitDesc* desc = g.desc; desc->type != CIRCUIT_END; desc++)
            {
                if(desc->u.instance.chip == chip_DIPSWITCH)
                {
                    DipswitchDesc* d = (DipswitchDesc*)desc->u.instance.custom_data;
                    Layout* l = new DipswitchSelector(d->desc, d->state, d->settings);
                    game_layout->append(*l, {~0, 0}, 10);
                }
                else if(desc->u.instance.chip == chip_DIPSWITCH_SP4T)
                {
                    DipswitchSP4TDesc* d = (DipswitchSP4TDesc*)desc->u.instance.custom_data;
                    Layout* l = new HexDipswitchSelector(d->desc, d->state, d->settings, 4);
                    game_layout->append(*l, {~0, 0}, 10);
                }
                else if(desc->u.instance.chip == chip_53137)
                {
                    Dipswitch53137Desc* d = (Dipswitch53137Desc*)desc->u.instance.custom_data;
                    Layout* l = new HexDipswitchSelector(d->desc, d->state, d->settings);
                    game_layout->append(*l, {~0, 0}, 10);
                }
                else if(desc->u.instance.chip == chip_DIPSWITCH_4SPST)
                {
                    Dipswitch4SP4TDesc* d = (Dipswitch4SP4TDesc*)desc->u.instance.custom_data;
                    Layout* l = new HexDipswitchSelector(d->desc, d->state, d->settings);
                    game_layout->append(*l, {~0, 0}, 10);
                }
                else if(desc->u.instance.chip == chip_POT_555_ASTABLE)
                {
                    PotentimeterAstable555Desc* d = (PotentimeterAstable555Desc*)desc->u.instance.custom_data;
                    Layout* l = new PotentiometerSelector(d->desc, d->current_val, d->min_val, d->max_val);
                    game_layout->append(*l, {~0, 0}, 10);
                }
                else if(desc->u.instance.chip == chip_POT_555_MONO)
                {
                    PotentimeterMono555Desc* d = (PotentimeterMono555Desc*)desc->u.instance.custom_data;
                    Layout* l = new PotentiometerSelector(d->desc, d->current_val, d->min_val, d->max_val);
                    game_layout->append(*l, {~0, 0}, 10);
                }
            }
        }

        game_view.onChange = [&]
        {
            game_configs[current_config].save();

            for(auto x : game_layouts)
                layout.remove(*x);

            current_config = game_view.selection();
            layout.append(*game_layouts[current_config], {~0, ~0});
        };

        layout.append(game_view, { 110, ~0 }, 10);

        exit_button.setText("Exit");
        exit_layout.append(exit_button, {350, 270 - exit_button.minimumSize().height, 90, exit_button.minimumSize().height});
    }

    void create(const Position& pos, int sel)
    {
        setGeometry({pos.x, pos.y, 450, 280});
        
        game_view.setSelection(sel);
        game_view.setSelected(true);
        game_view.onChange();

        append(layout);
        append(exit_layout);
        setVisible(true);
        game_view.setFocused();
        //setModal(true);
    }
};
