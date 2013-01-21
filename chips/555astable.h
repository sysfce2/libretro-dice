#ifndef ASTABLE_555_H
#define ASTABLE_555_H

#include "../chip_desc.h"

class Astable555Desc
{
public:
	double r1, r2, c;

    static const double LN_2;

	Astable555Desc(double res1, double res2, double cap) : r1(res1), r2(res2), c(cap) { };
	static CUSTOM_LOGIC( astable_555 );
};

extern CHIP_DESC( 555_Astable );

#endif
