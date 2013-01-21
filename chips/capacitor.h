#ifndef CAPACITOR_H
#define CAPACITOR_H

#include "../chip_desc.h"

class CapacitorDesc
{
public:
    double r, c;

    CapacitorDesc(double cap, double res = 0.0) : c(cap), r(res) { }

    static CUSTOM_LOGIC( capacitor );
};

extern CHIP_DESC( CAPACITOR );


class SeriesRCDesc
{
public:
    double r, c;

    SeriesRCDesc(double res, double cap) : r(res), c(cap) { }

    static CUSTOM_LOGIC( series_rc );
};

extern CHIP_DESC( SERIES_RC );



class BufferDesc
{
public:
    double tp_lh, tp_hl;

    BufferDesc(double lh, double hl) : tp_lh(lh), tp_hl(hl) { }

    static CUSTOM_LOGIC( buffer );
};

extern CHIP_DESC( BUFFER );


#endif
