#include "ram.h"

#include "io.h"

void ram_init()
{
    set_MREQ(1);
    set_WR(1);
    set_RD(1);
    set_data_as_input();
    set_addr_as_input();
}
