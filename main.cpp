#include <phoenix.hpp>
#include <nall/platform.hpp>
#include <nall/directory.hpp>
#include "manymouse/manymouse.h"

using namespace nall;
using namespace phoenix;

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include "circuit.h"
#include "circuit_desc.h"

#include "game_list.h"

#include "chips/input.h"
#include "ui/audio_window.h"
#include "ui/input_window.h"
#include "ui/dipswitch_window.h"
#include "ui/game_window.h"
#include "ui/logo.h"

#define DEBUG
#undef DEBUG

static const char VERSION_STRING[] = "DICE 0.6";

struct Application : Window 
{
    Settings settings;
    Input input;
    Circuit* circuit;
    
    // Game menu
    Menu game_menu;
    GameWindow game_window;  
    Item new_game_item, end_game_item;
    Separator game_sep[2];
    CheckItem pause_item, throttle_item;
    Item exit_item;

    // Settings menu
    Menu settings_menu;
    Item audio_item;
    CheckItem mute_item;
    AudioWindow audio_window;
    Separator settings_sep[3];
    CheckItem fullscreen_item, keep_aspect_item;
    Item input_item;
    InputWindow input_window;
    Item dipswitch_item;
    DipswitchWindow dipswitch_window;
    
    VerticalLayout layout;
    Viewport viewport;

    struct UserInterfaceState
    {
        bool pause, throttle, fullscreen, quit;

        static UserInterfaceState getCurrent(Application& application)
        {
            return { application.input.getKeyPressed(application.settings.input.ui.pause),
                     application.input.getKeyPressed(application.settings.input.ui.throttle),
                     application.input.getKeyPressed(application.settings.input.ui.fullscreen),
                     application.input.getKeyPressed(application.settings.input.ui.quit) };
        }
    };
    UserInterfaceState prev_ui_state;

    Application() : audio_window(settings, mute_item), input_window(settings), circuit(nullptr), prev_ui_state{false, false, false, false}
    {
        // Load config file
        nall::string config_path = configpath();
        config_path.append("dice/");
        directory::create(config_path);

        settings.filename = {config_path, "settings.cfg"};
        settings.load();
        
        onClose = [&] { &OS::quit; SDL_Quit(); exit(0); };

        // Game menu
        game_menu.setText("Game");

        new_game_item.setText("New Game...");
        new_game_item.onActivate = [&] { game_window.create(geometry().position()); };
        game_window.cancel_button.onActivate = [&] { game_window.setVisible(false); setFocused(); };
        
        game_window.start_button.onActivate = [&]
        {
            GameDesc& g = game_list[game_window.game_view.selection()];
            if(circuit) delete circuit; 
            circuit = new Circuit(settings, input, g.desc, g.command_line);
            game_window.setVisible(false);
            setFocused();
            onSize();
        };

        game_menu.append(new_game_item);

        end_game_item.setText("End Game");
        end_game_item.onActivate = [&] 
        { 
            if(circuit) 
            {
                delete circuit; 
                circuit = nullptr;
            }
            onSize();
        };
        game_menu.append(end_game_item);

        game_menu.append(game_sep[0]);
        pause_item.setText("Pause");
        pause_item.onToggle = [&] { settings.pause = pause_item.checked(); };
        throttle_item.setText("Throttle");
        throttle_item.setChecked(true);
        throttle_item.onToggle = [&]
        { 
            settings.throttle = throttle_item.checked();
            if(settings.throttle && circuit) // Adjust rtc
            {
                uint64_t emu_time = circuit->global_time * 1000000.0 * Circuit::timescale;
                circuit->rtc += int64_t(circuit->rtc.get_usecs()) - emu_time;
            }
        };
        game_menu.append(pause_item, throttle_item);

        game_menu.append(game_sep[1]);
        exit_item.setText("Exit");
        exit_item.onActivate = onClose;
        game_menu.append(exit_item);

        append(game_menu);

        // Settings menu
        settings_menu.setText("Settings");

        audio_item.setText("Audio Settings...");
        settings_menu.append(audio_item);
        audio_item.onActivate = [&] { audio_window.create(geometry().position()); };
        audio_window.onClose = audio_window.exit_button.onActivate = [&] 
        {
            mute_item.setChecked(settings.audio.mute);
            settings.save();
            audio_window.setVisible(false);
            setFocused();
            if(circuit) circuit->audio.audio_init(circuit);
        };
        mute_item.setText("Mute Audio");
        mute_item.setChecked(settings.audio.mute);
        mute_item.onToggle = [&] { settings.audio.mute = mute_item.checked(); if(circuit) circuit->audio.audio_init(circuit); };

        settings_menu.append(mute_item);
        settings_menu.append(settings_sep[0]);

        fullscreen_item.setText("Fullscreen");
        fullscreen_item.onToggle = [&] { toggleFullscreen(fullscreen_item.checked()); };
        keep_aspect_item.setText("Keep Aspect Ratio");
        keep_aspect_item.setChecked(settings.video.keep_aspect);
        keep_aspect_item.onToggle = [&] 
        { 
            settings.video.keep_aspect = keep_aspect_item.checked();
            onSize(); 
        };
        settings_menu.append(fullscreen_item, keep_aspect_item);

        settings_menu.append(settings_sep[1]);
        input_item.setText("Configure Inputs...");
        input_item.onActivate = [&] { input_window.create(geometry().position()); };
        input_window.onClose = [&] 
        {
            if(input_window.active_selector) input_window.active_selector->assign(KeyAssignment::None);
            settings.save(); 
            input_window.setVisible(false);
            setFocused();
        };
        input_window.exit_button.onActivate = [&]
        {
            if(input_window.active_selector) 
                input_window.active_selector->assign(KeyAssignment::None);
            else
            {
                settings.save(); 
                input_window.setVisible(false);
                setFocused();
            }
        };

        settings_menu.append(input_item);

        dipswitch_item.setText("Configure DIP Switches...");
        dipswitch_item.onActivate = [&] 
        { 
            int selection = 0;
            if(circuit) for(int i = 0; i < dipswitch_window.game_configs.size(); i++)
            {            
                if(circuit->game_config == dipswitch_window.game_configs[i])
                {
                    selection = i;
                    break;
                }
            }
            dipswitch_window.create(geometry().position(), selection); 
        };
        dipswitch_window.onClose = dipswitch_window.exit_button.onActivate = [&] 
        {
            dipswitch_window.game_configs[dipswitch_window.current_config].save();
            dipswitch_window.setVisible(false);
            setFocused();
        };
        settings_menu.append(settings_sep[2]);
        settings_menu.append(dipswitch_item);

        append(settings_menu);

        setStatusVisible();

        setBackgroundColor({0, 0, 0});
        layout.setMargin(0);
        layout.append(viewport, {~0, ~0});
        append(layout);

        // Initialize SDL, input, etc.
        settings.num_mice = ManyMouse_Init();

        char env[256];
        sprintf(env, "SDL_WINDOWID=%lld", (uint64_t)viewport.handle());
        putenv(env);

        if(SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO|SDL_INIT_JOYSTICK) < 0)
	    {
	        printf("Unable to init SDL:\n%s\n", SDL_GetError());
		    exit(1);
	    }

        input.init();


        onSize = [&] {

            if((signed)geometry().height < 0 || (signed)geometry().width < 0)
                return;
            
            if(circuit == nullptr)
            {
                drawLogo();
            }
            else
            {
                circuit->video.video_init(geometry().width, geometry().height, circuit);
            }

            viewport.setFocused();
        };

        setTitle(VERSION_STRING);
        setFrameGeometry({(Desktop::workspace().width - 640) / 2, (Desktop::workspace().height - 480) / 2, 640, 480});
        setMenuVisible();
        setVisible();

        onSize();
    }

    void toggleFullscreen(bool fullscreen)
    {
        settings.fullscreen = fullscreen;
        fullscreen_item.setChecked(fullscreen);
        setStatusVisible(!fullscreen);
        setMenuVisible(!fullscreen);
        setFullScreen(fullscreen);
        SDL_ShowCursor(!fullscreen);
        onSize();
    }

    void drawLogo()
    {
        //Set OpenGL Parameters
	    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	    //SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
        
        if(SDL_SetVideoMode(geometry().width, geometry().height, 32, SDL_OPENGL) == NULL)
	    {
		    printf("Unable to set video mode:\n%s\n", SDL_GetError());
		    exit(1);
	    }

  	    glViewport(0, 0, geometry().width, geometry().height);

        glMatrixMode(GL_PROJECTION);
	    glLoadIdentity();
	    glOrtho(0.0, geometry().width, geometry().height, 0.0, -1.0, 1.0);
	    glMatrixMode(GL_MODELVIEW);
	    glLoadIdentity();

	    glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);  

        glEnable(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        
        unsigned logo_width = logo_data[0x12];
        unsigned logo_height = logo_data[0x16];
        int logo_x = (geometry().width - logo_width*4) / 2;
        int logo_y = (geometry().height - logo_height*4) / 2;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, logo_width, logo_height, 0, GL_RGB, GL_UNSIGNED_BYTE, &logo_data[0x36]);

        glBindTexture (GL_TEXTURE_2D, 0);
        glBegin(GL_QUADS);
            glColor3f(1.0, 1.0, 1.0);
            glTexCoord2f(0.0, 1.0);
            glVertex3f(logo_x + 0.0, logo_y + 0.0, 0.0);
            glTexCoord2f(1.0, 1.0);
            glVertex3f(logo_x + logo_width*4, logo_y + 0.0, 0.0);
            glTexCoord2f(1.0, 0.0);
            glVertex3f(logo_x + logo_width*4, logo_y + logo_height*4, 0.0);
            glTexCoord2f(0.0, 0.0);
            glVertex3f(logo_x + 0.0, logo_y + logo_height*4, 0.0);
        glEnd();

        SDL_GL_SwapBuffers();
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    }

    void run()
    {
        RealTimeClock real_time;
        
        while(1)
        {
            input.poll_input();

            if(circuit && !settings.pause)
            {
                circuit->run(2.5e-3 / Circuit::timescale); // Run 2.5 ms

                uint64_t emu_time = circuit->global_time * 1000000.0 * Circuit::timescale;
                
                // Make sure real time is at least mostly caught up
                if(settings.throttle)
                    while(circuit->rtc.get_usecs() + 50000 < emu_time);

                // If real time is too far ahead, adjust timer
                if(circuit->rtc.get_usecs() > emu_time + 100000)
                    circuit->rtc += (circuit->rtc.get_usecs() - emu_time - 100000);

                if(real_time.get_usecs() > 1000000)
                {
                    setStatusText({"FPS: ", circuit->video.frame_count});
                    circuit->video.frame_count = 0;
                    real_time += 1000000;
                }

                //if(emu_time > 3.040e6) break;
            }
            else
            {
                SDL_Delay(5);
                if(settings.pause && statusText() != "Paused") setStatusText("Paused");
                else if(!settings.pause && statusText() != VERSION_STRING) setStatusText(VERSION_STRING);   
            }

            if(input_window.active_selector)
            {
                auto key_state = Keyboard::state();
        
                for(unsigned i = 0; i < key_state.size(); i++)
                    if(key_state[i]) input_window.active_selector->assign({KeyAssignment::KEYBOARD, i});

                //Check Joystick Events
                SDL_Event event;
                while(SDL_PollEvent(&event))
                {
                    switch(event.type)
                    {   
                        case SDL_JOYBUTTONDOWN:
                            input_window.active_selector->assign({KeyAssignment::JOYSTICK_BUTTON, event.jbutton.button, event.jbutton.which});
                            break;
                        case SDL_JOYAXISMOTION:
                            if(abs(event.jaxis.value) > 8192)
                            {
                                unsigned axis = (event.jaxis.axis << 1) | (event.jaxis.value > 0);
                                input_window.active_selector->assign({KeyAssignment::JOYSTICK_AXIS, axis, event.jaxis.which});
                            }
                        default:
                            break;
                    }
                }
            }
            else // Handle user interface inputs
            {            
                UserInterfaceState ui_state = UserInterfaceState::getCurrent(*this);

                // Don't update ui inputs while main window is in the background
                if(focused())
                {            
                    if(ui_state.quit && !prev_ui_state.quit)
                    {
                        //if(settings.no_gui)
                        //    onClose();
                        //else if(settings.fullscreen)
                        //    toggleFullscreen(false);
                        //else
                            //onClose();

                        return;
                    }
                    if(ui_state.pause && !prev_ui_state.pause)
                    {
                        settings.pause = !settings.pause;
                        pause_item.setChecked(settings.pause);
                    }
                    if(ui_state.fullscreen && !prev_ui_state.fullscreen)
                    {
                        fullscreen_item.setChecked(!settings.fullscreen);
                        fullscreen_item.onToggle();
                    }
                    if(ui_state.throttle && !prev_ui_state.throttle)
                    {
                        throttle_item.setChecked(!settings.throttle);
                        throttle_item.onToggle();
                    }
                }

                prev_ui_state = ui_state;
            }
        }
    }
};

static nall::string app_path;
static Application* app_ptr;

const nall::string& application_path()
{
    return app_path;
}

Window& application_window()
{
    return *app_ptr;
}

#undef main
int main(int argc, char** argv)
{
    // Sort the game list
    std::sort(game_list, game_list + game_list_size);
    
    Application application;

    // Init application path/window for ROM loader
    app_ptr = &application;
    app_path = dir(realpath(argv[0]));

    // Seed random number generator
    srand(time(NULL));

    if(argc > 1)
    {
        for(const GameDesc& g : game_list)
        {
            if(strcmp(argv[1], g.command_line) == 0)
            {
                application.circuit = new Circuit(application.settings, application.input, g.desc, g.command_line);
                application.settings.no_gui = true;
                application.toggleFullscreen(true);
            }
        }
    }

    application.run();

#ifdef DEBUG
    printf("chips: %d\n", application.circuit->chips.size());
    uint64_t total_event_count = 0;
    uint64_t total_loop_count[8] = { 0 };
	for(int i = 0; i < application.circuit->chips.size(); i++)
	{
		total_event_count += application.circuit->chips[i]->total_event_count;
        printf("%p LUT=%llx, event_count=%lld, act_count=%lld, state=%d act_out:%d cyc_time:%lld act_time:%lld\n",
               application.circuit->chips[i],
			   (uint32_t)application.circuit->chips[i]->lut_data, 
			   application.circuit->chips[i]->total_event_count,
               application.circuit->chips[i]->activation_count,
			   application.circuit->chips[i]->state,
               application.circuit->chips[i]->active_outputs,
               application.circuit->chips[i]->cycle_time,
               application.circuit->chips[i]->activation_time);
        application.circuit->chips[i]->print_input_events();
        application.circuit->chips[i]->print_output_events();
        for(int j = 0; j < 8; j++) 
        {
            printf("loop_count%d:%lld, ", j, application.circuit->chips[i]->loop_count[j]);
            total_loop_count[j] += application.circuit->chips[i]->loop_count[j];
        }
        printf("\n");
	}
    printf("total event count:%lld\n", total_event_count);
    for(int j = 0; j < 8; j++) 
    {
        printf("total_loop_count%d:%lld, ", j, total_loop_count[j]);
    }
    printf("\n");
#endif

    return 0;
}

