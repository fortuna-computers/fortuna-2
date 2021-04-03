#include "z80.h"

#include <stdio.h>

#include "io.h"

void z80_init()
{
    set_RST(0);
    set_BUSREQ(1);
    set_CLK(0);
    set_INT(1);
}

void z80_cycle()
{
    set_CLK(1);
    set_CLK(0);
}
