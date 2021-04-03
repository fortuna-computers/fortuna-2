#include "tests.h"

#ifdef DEBUG

#include "io.h"

void run_tests()
{
    set_addr_as_output();
    set_addr(0x1e2);
    set_data_as_output();
    set_data(0x43);

    for(;;);
}

#endif
