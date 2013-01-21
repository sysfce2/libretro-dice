#ifndef VCD_LOG_H
#define VCD_LOG_H

#include "../chip_desc.h"

#include <cstdio>
#include <map>

/* 
    VCD Log:
    
    Outputs Waveform data in the VCD file format.
*/

class VcdLogDesc
{
private:
	FILE* file;
	std::map<int, char> vars;

public:
	VcdLogDesc(const char* filename, 
			   int num1 = 1, const char* name1 = NULL,
			   int num2 = 2, const char* name2 = NULL,
			   int num3 = 3, const char* name3 = NULL,
			   int num4 = 4, const char* name4 = NULL,
			   int num5 = 5, const char* name5 = NULL,
			   int num6 = 6, const char* name6 = NULL,
			   int num7 = 7, const char* name7 = NULL,
			   int num8 = 8, const char* name8 = NULL,
			   int num9 = 9, const char* name9 = NULL,
			   int num10 = 10, const char* name10 = NULL,
			   int num11 = 11, const char* name11 = NULL,
			   int num12 = 12, const char* name12 = NULL,
			   int num13 = 13, const char* name13 = NULL,
			   int num14 = 14, const char* name14 = NULL,
			   int num15 = 15, const char* name15 = NULL,
               int num16 = 16, const char* name16 = NULL);

    ~VcdLogDesc();

    static CUSTOM_LOGIC( vcd_log );
};

extern CHIP_DESC( VCD_LOG );

#endif
