#include "../chips/dipswitch.h"

struct GameWindow : Window
{
    VerticalLayout layout;
    ListView game_view;
    
    HorizontalLayout button_layout;
    Button start_button;
    Button cancel_button;
    Label spacer;

    GameWindow()
    {
        layout.setMargin(10);
        game_view.setHeaderText("Game", "Manufacturer", "Year");
        game_view.setHeaderVisible();

        for(const GameDesc& g : game_list)
            game_view.append(g.name, g.manufacturer, g.year);

        game_view.autoSizeColumns();

        start_button.setText("Start Game");
        cancel_button.setText("Cancel");
        //cancel_button.onActivate = [&] { setVisible(false); };

        button_layout.append(start_button, {100, 0});
        button_layout.append(spacer, {~0, ~0});
        button_layout.append(cancel_button, {100, 0});

        layout.append(game_view, {~0, ~0}, 10);
        layout.append(button_layout, {~0, 0});
    }

    void create(const Position& pos)
    {
        setGeometry({pos.x, pos.y, 300, 300});
        setTitle("New Game");
        setResizable(false);
        setModal(true);
        
        game_view.setSelection(0);
        game_view.setSelected(true);
        //game_view.onChange();

        append(layout);
        setVisible(true);

        game_view.setFocused();
    }
};
