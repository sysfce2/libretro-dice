#include <string>

struct RomDesc
{
    std::string file_name;
    std::string rom_name;
    uint32_t crc;

    RomDesc(std::string f, std::string r, uint32_t c) : file_name(f), rom_name(r), crc(c) { }

    //
    // Caches last used ROM file for faster LUT initialization
    //
    static uint8_t get_data(const RomDesc* rom, unsigned offset);
};

