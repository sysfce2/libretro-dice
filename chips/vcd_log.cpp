
#include <ctime>

#include "vcd_log.h"
#include "../circuit.h"

/* 
    VCD Log:
    
    Outputs Waveform data in the VCD file format.
*/

//TODO: Fix when multiple games try to use same file name, constructor is
// always being called even when game isn't instantiated 

VcdLogDesc::VcdLogDesc(const char* filename, 
                       int num1, const char* name1,
                       int num2, const char* name2,
                       int num3, const char* name3,
                       int num4, const char* name4,
                       int num5, const char* name5,
                       int num6, const char* name6,
                       int num7, const char* name7,
                       int num8, const char* name8,
                       int num9, const char* name9,
                       int num10, const char* name10,
                       int num11, const char* name11,
                       int num12, const char* name12,
                       int num13, const char* name13,
                       int num14, const char* name14,
                       int num15, const char* name15,
                       int num16, const char* name16)
{
	file = fopen(filename, "w");
	fprintf(file, "$date\n");

	time_t realtime;
	time(&realtime);
	fprintf(file, "%s", ctime(&realtime));
	fprintf(file, "$end\n");

	fprintf(file, "$version\npongsim 0.01\n$end\n");
	fprintf(file, "$timescale 1ps $end\n");
	fprintf(file, "$scope module pongsim $end\n");

	char c = 'a';
	if(name1) fprintf(file, "$var wire 1 %c %s $end\n", c, name1); vars[1 << (num1-1)] = c++;
	if(name2) fprintf(file, "$var wire 1 %c %s $end\n", c, name2); vars[1 << (num2-1)] = c++;
	if(name3) fprintf(file, "$var wire 1 %c %s $end\n", c, name3); vars[1 << (num3-1)] = c++;
	if(name4) fprintf(file, "$var wire 1 %c %s $end\n", c, name4); vars[1 << (num4-1)] = c++;
	if(name5) fprintf(file, "$var wire 1 %c %s $end\n", c, name5); vars[1 << (num5-1)] = c++;
	if(name6) fprintf(file, "$var wire 1 %c %s $end\n", c, name6); vars[1 << (num6-1)] = c++;
	if(name7) fprintf(file, "$var wire 1 %c %s $end\n", c, name7); vars[1 << (num7-1)] = c++;
	if(name8) fprintf(file, "$var wire 1 %c %s $end\n", c, name8); vars[1 << (num8-1)] = c++;
	if(name9) fprintf(file, "$var wire 1 %c %s $end\n", c, name9); vars[1 << (num9-1)] = c++;
	if(name10) fprintf(file, "$var wire 1 %c %s $end\n", c, name10); vars[1 << (num10-1)] = c++;
	if(name11) fprintf(file, "$var wire 1 %c %s $end\n", c, name11); vars[1 << (num11-1)] = c++;
	if(name12) fprintf(file, "$var wire 1 %c %s $end\n", c, name12); vars[1 << (num12-1)] = c++;
	if(name13) fprintf(file, "$var wire 1 %c %s $end\n", c, name13); vars[1 << (num13-1)] = c++;
	if(name14) fprintf(file, "$var wire 1 %c %s $end\n", c, name14); vars[1 << (num14-1)] = c++;
	if(name15) fprintf(file, "$var wire 1 %c %s $end\n", c, name15); vars[1 << (num15-1)] = c++;
    if(name16) fprintf(file, "$var wire 1 %c %s $end\n", c, name16); vars[1 << (num16-1)] = c++;

	fprintf(file, "$upscope $end\n");
	fprintf(file, "$enddefinitions $end\n");
	fprintf(file, "$dumpvars\n");		
}
	
VcdLogDesc::~VcdLogDesc()
{
	fclose(file);
}

CUSTOM_LOGIC( VcdLogDesc::vcd_log )
{
	VcdLogDesc* desc = (VcdLogDesc*)chip->custom_data;
	
	chip->inputs ^= mask;

	if(desc == NULL) return;

    if(mask == 0) return;
    
    fprintf(desc->file, "#%lld\n", chip->circuit->global_time);

    for(int m = ~0; mask & m;)
    {
        int i = 1 << __builtin_ctz(mask & m);
        fprintf(desc->file, "%d%c\n", (chip->inputs & i) ? 1 : 0, desc->vars[i]);
        m &= ~i;
    }

	//fprintf(desc->file, "#%lld\n", chip->circuit->global_time);
	//fprintf(desc->file, "%d%c\n", (chip->inputs & mask) ? 1 : 0, desc->vars[mask]);
}


CHIP_DESC( VCD_LOG ) = 
{
	CUSTOM_CHIP_START(&VcdLogDesc::vcd_log)
	    INPUT_PINS(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16),

	CHIP_DESC_END
};
