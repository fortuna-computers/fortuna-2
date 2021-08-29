#include "step.h"

#ifdef STEP

#include <stdbool.h>
#include <stdio.h>
#include <avr/pgmspace.h>

#include "io.h"
#include "ram.h"
#include "serial.h"
#include "z80.h"

extern uint16_t last_pressed_key;

void step_init()
{
    putchar('\n');
    for (int i = 0; i < 24; ++i) {
        printf_P(PSTR("%02X "), buffer[i]);
    }
    printf_P(PSTR("\n\nADDR DATA  MREQ WR RD IORQ  INT BUSRQ BUSAK  RST\n"));
}

static const char* bit(bool value)
{
    if (value)
        return GRN "1" RESET;
    else
        return RED "0" RESET;
}

void step()
{
    switch (getchar()) {
        case 'k':
            last_pressed_key = 'k';
            z80_interrupt(KEYBOARD_DEVICE);
            break;
        default:
            z80_cycle();
            char addr[5] = "----",
                 data[3] = "--";
            if (get_MREQ() == 0 || get_IORQ() == 0) {
                sprintf(addr, "%04X", get_addr());
                sprintf(data, "%02X", get_data());
            }
            printf_P(PSTR("%s  %s     %s   %s  %s   %s    %s     %s     %s    %s \n"),
                    addr, data, bit(get_MREQ()), bit(get_WR()), bit(get_RD()), bit(get_IORQ()),
                    bit(get_INT()), bit(get_BUSREQ()), bit(get_BUSACK()), bit(get_RST()));
    }
}

#endif
