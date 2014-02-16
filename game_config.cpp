#include <nall/platform.hpp>
#include <nall/directory.hpp>

#include "game_config.h"
#include "chips/dipswitch.h"

using namespace nall;

GameConfig::GameConfig(const CircuitDesc* desc, const char* name)
{
    // Load config file
    nall::string config_path = configpath();
    config_path.append("dice/");
    directory::create(config_path);

    filename = {config_path, name, ".cfg"};

    bool has_config = false;

    // Find all DIP switches in circuit desc, append to game configuration
    while(desc->type != CIRCUIT_END)
    {
        if(desc->type == CHIP_INST)
        {
            if(desc->u.instance.chip == chip_DIPSWITCH)
            {
                DipswitchDesc* d = (DipswitchDesc*)desc->u.instance.custom_data;
                append(d->state, d->name);
                has_config = true;
            }
            else if(desc->u.instance.chip == chip_DIPSWITCH_SP4T)
            {
                DipswitchSP4TDesc* d = (DipswitchSP4TDesc*)desc->u.instance.custom_data;
                append(d->state, d->name);
                has_config = true;
            }
            else if(desc->u.instance.chip == chip_53137)
            {
                Dipswitch53137Desc* d = (Dipswitch53137Desc*)desc->u.instance.custom_data;
                append(d->state, d->name);
                has_config = true;
            }
            else if(desc->u.instance.chip == chip_DIPSWITCH_4SPST)
            {
                Dipswitch4SP4TDesc* d = (Dipswitch4SP4TDesc*)desc->u.instance.custom_data;
                append(d->state, d->name);
                has_config = true;
            }
            else if(desc->u.instance.chip == chip_POT_555_ASTABLE)
            {
                PotentimeterAstable555Desc* d = (PotentimeterAstable555Desc*)desc->u.instance.custom_data;
                append(d->current_val, d->name);
                has_config = true;
            }
            else if(desc->u.instance.chip == chip_POT_555_MONO)
            {
                PotentimeterMono555Desc* d = (PotentimeterMono555Desc*)desc->u.instance.custom_data;
                append(d->current_val, d->name);
                has_config = true;
            }
        }

        desc++;
    }

    if(has_config) load();
}
