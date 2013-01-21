struct AudioWindow : Window
{
    Settings& settings;
    
    VerticalLayout layout;
    HorizontalLayout freq_layout, vol_layout;
    Label freq_label, vol_label;
    LineEdit vol_val;
    HorizontalSlider vol_slider;
    CheckBox mute_checkbox;
    HorizontalLayout exit_layout;
    Button exit_button;
    Widget offset;

    RadioBox freq_options[4];

    AudioWindow(Settings& s, CheckItem& i) : settings(s)
    {
        layout.setMargin(10);
        
        freq_label.setText("Frequency:");
        freq_options[0].setText("22050 Hz");
        freq_options[1].setText("44100 Hz");
        freq_options[2].setText("48000 Hz");
        freq_options[3].setText("96000 Hz");

        freq_layout.append(freq_label, {0, 0}, 10);
        freq_layout.append(freq_options[0], {0, 0}, 10);
        freq_layout.append(freq_options[1], {0, 0}, 10);
        freq_layout.append(freq_options[2], {0, 0}, 10);
        freq_layout.append(freq_options[3], {0, 0});
        RadioBox::group(freq_options[0], freq_options[1], freq_options[2], freq_options[3]);

        for(RadioBox& x : freq_options)
        {
            x.onActivate = [&] 
            {
                for(int i = 0; i < 4; i++)
                    if(freq_options[i].checked())
                        settings.audio.frequency = i;
            };
        }        

        vol_label.setText("Volume:");
        vol_layout.append(vol_label, {0, 0}, 10);
        
        vol_val.setEditable(false);
        vol_layout.append(vol_val, {45, 0}, 10);

        vol_slider.setLength(1001);
        vol_slider.onChange = [&] 
        { 
            settings.audio.volume = vol_slider.position();
            vol_val.setText({settings.audio.volume / 10, "%"});
        };
        vol_layout.append(vol_slider, {~0, 30});

        layout.append(freq_layout, {~0, 0}, 10);
        layout.append(vol_layout, {~0, 0}, 10);

        mute_checkbox.setText("Mute Audio");
        mute_checkbox.onToggle = [&]
        { 
            vol_slider.setEnabled(!mute_checkbox.checked()); 
            vol_val.setEnabled(!mute_checkbox.checked());
            settings.audio.mute = mute_checkbox.checked();
        };
        layout.append(mute_checkbox, {0, 0}, 10);

        exit_button.setText("Exit");
        //exit_button.onActivate = onClose = [&] { settings.save(); setVisible(false); };
        //exit_button.onActivate = onClose;
        exit_layout.append(offset, {315, 0});
        exit_layout.append(exit_button, {~0, 0});
        layout.append(exit_layout, {~0, 0});

    }
    
    void create(const Position& pos)
    {
        setGeometry({pos.x, pos.y, 425, layout.minimumGeometry().height});
        setTitle("Audio Settings");
        setResizable(false);
        setModal(true);

        if(settings.audio.frequency <= 3)
            freq_options[settings.audio.frequency].setChecked();

        mute_checkbox.setChecked(settings.audio.mute);
        vol_slider.setEnabled(!settings.audio.mute); 
        vol_val.setEnabled(!settings.audio.mute);

        vol_slider.setPosition(settings.audio.volume);
        vol_val.setText({settings.audio.volume / 10, "%"});
       
        append(layout);

        setVisible(true);
    }
};

