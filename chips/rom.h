#ifndef ROM_H__
#define ROM_H__

#include <string>
#include <cstdint>

struct RomDesc
{
    std::string file_name;
    std::string rom_name;
    uint32_t crc, alt_crc;

    RomDesc(std::string f, std::string r, uint32_t c) : file_name(f), rom_name(r), crc(c), alt_crc(c) { }
    RomDesc(std::string f, std::string r, uint32_t c, uint32_t a) : file_name(f), rom_name(r), crc(c), alt_crc(a) { }

    //
    // Caches last used ROM file for faster LUT initialization
    //
    static uint8_t get_data(const RomDesc* rom, unsigned offset);
   
   static void set_zip_filename(const char *filename);
};

#endif
