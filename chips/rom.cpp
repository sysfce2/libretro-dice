#include <nall/platform.hpp>
#include <nall/zip.hpp>
#include <phoenix.hpp>

#include "rom.h"

using namespace nall;
using namespace phoenix;

// in main.cpp
extern const nall::string& application_path();
extern Window& application_window();

static nall::string filename;
static nall::string romname;

static zip zip_file;
static nall::vector<uint8_t> rom_data;

// TODO: Prevent multiple error popups when file is not found.
// By extracting entire file at once?

uint8_t RomDesc::get_data(const RomDesc* rom, unsigned offset)
{
    if(filename != rom->file_name.c_str())
    {
        filename = rom->file_name.c_str();
        
        if(zip_file.open({application_path(), "roms\\", filename, ".zip"}) == false)
        {
            MessageWindow::critical(application_window(), {"ROM File roms/", filename, ".zip not found.\n"
                                                   "Game will not function correctly!"});

            romname = rom->rom_name.c_str();
            return 0xff;
        }
    }

    if(romname != rom->rom_name.c_str())
    {
        romname = rom->rom_name.c_str();

        rom_data.reset();
        
        // Check for rom with correct CRC
        for(zip::File& f : zip_file.file)
            if(f.crc32 == rom->crc)
            {
                rom_data = zip_file.extract(f);
                break;
            }

        if(rom_data.size() == 0) // ROM w/ correct CRC not found, check for ROM with correct name
        {
            for(zip::File& f : zip_file.file)
                if(f.name == romname)
                {
                    rom_data = zip_file.extract(f);
                    MessageWindow::warning(application_window(), {"ROM ", romname, " incorrect CRC.\n"
                                           "Expected: ", hex(rom->crc), " Found: ", hex(f.crc32),
                                           "\nGame may not function correctly."});
                    break;
                }
        }

        if(rom_data.size() == 0)
        {
            MessageWindow::critical(application_window(), {"ROM ", romname, " with CRC ",
                                    hex(rom->crc), " not found.\n"
                                    "Game will not function correctly!"});
        }
    }

    if(offset < rom_data.size()) return rom_data[offset];

    return 0xff;
}

