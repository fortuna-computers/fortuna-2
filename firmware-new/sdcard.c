#include "sdcard.h"

#include <stddef.h>
#include <util/delay.h>

#include "ram.h"
#include "spi.h"

#define CMD0   0
#define CMD8   8
#define CMD17  17
#define CMD24  24
#define CMD55  55
#define CMD58  58
#define ACMD41 41

#define MAX_READ_ATTEMPTS 20
#define MAX_WRITE_ATTEMPTS 100

extern volatile uint8_t buffer[512];

//
// STATIC FUNCTIONS
//

static SDCardStage last_stage    = SD_NOT_INITIALIZED;
static uint8_t     last_response = 0xff;

static void reset()
{
    // powerup card
    spi_deactivate();
    _delay_ms(1);
    for (uint8_t i = 0; i < 10; ++i)
        spi_send(0xff);
    
    // deselect card
    spi_deactivate();
    spi_send(0xff);
}

static void command(uint8_t cmd, uint32_t args, uint8_t crc)
{
    spi_send(cmd | 0x40);
    spi_send((uint8_t)(args >> 24));
    spi_send((uint8_t)(args >> 16));
    spi_send((uint8_t)(args >> 8));
    spi_send((uint8_t)args);
    spi_send(crc | 0x1);
}

static uint8_t go_idle()
{
    spi_activate();
    command(CMD0, 0, 0x94);
    uint8_t r1 = spi_recv_ignore_ff();
    spi_deactivate();
    return r1;
}

static uint8_t if_cond(uint32_t* response)
{
    spi_activate();
    command(CMD8, 0x1AA, 0x86);
    uint8_t r1 = spi_recv_ignore_ff();
    if (r1 <= 1) {
        *response = 0;
        *response |= (uint32_t) spi_send(0xff) << 24;
        *response |= (uint32_t) spi_send(0xff) << 16;
        *response |= (uint32_t) spi_send(0xff) << 8;
        *response |= (uint32_t) spi_send(0xff);
    }
    spi_deactivate();
    return r1;
}

static uint8_t init_process(uint32_t* response)
{
    spi_activate();
    command(CMD55, 0, 0);
    uint8_t r1 = spi_recv_ignore_ff();
    spi_deactivate();
    if (r1 > 1)
        return r1;
    
    spi_activate();
    command(ACMD41, 0x40000000, 0);
    r1 = spi_recv_ignore_ff();
    if (r1 <= 1) {
        *response = 0;
        *response |= (uint32_t) spi_send(0xff) << 24;
        *response |= (uint32_t) spi_send(0xff) << 16;
        *response |= (uint32_t) spi_send(0xff) << 8;
        *response |= (uint32_t) spi_send(0xff);
    }
    spi_deactivate();
    
    return r1;
}

static bool initialize_once()
{
    last_response = 0xff;
    last_stage = SD_RESET;
    reset();
    
    last_stage = SD_GO_IDLE;
    last_response = go_idle();
    if (last_response != 0x1)
        return false;
    
    uint32_t response = 0;
    last_stage = SD_IF_COND;
    last_response = if_cond(&response);
    if (last_response > 1)
        return false;
    
    last_stage = SD_INIT;
    for (int i = 0; i < 16; ++i) {
        last_response = init_process(&response);
        if (last_response == 0) {
            return true;
        }
        _delay_ms(50);
    }
    return false;
}


//
// PUBLIC FUNCTIONS
//

bool sdcard_initialize()
{
    // will attempt to initialize 10 times
    for (size_t i = 0; i < 10; ++i) {
        if (initialize_once())
            return true;
        _delay_ms(80);
    }
    return false;
}

bool sdcard_read_page(uint32_t block)
{
    spi_activate();
    
    // send read command
    command(CMD17, block, 0);
    uint8_t r = spi_recv_ignore_ff();
    last_response = r;
    if (r != 0) {
        spi_deactivate();
        last_stage = SD_READ_REJECTED;
        return false;
    }
    
    // read data
    uint8_t rr = 0;
    for (int i = 0; i < MAX_READ_ATTEMPTS; ++i) {
        rr = spi_recv_ignore_ff();
        if (rr == 0xfe)
            goto read_data;
        _delay_ms(10);
    }
    
    // read timeout
    spi_deactivate();
    last_stage = SD_READ_TIMEOUT;
    return false;

read_data:
    for (int i = 0; i < 512; ++i)
        buffer[i] = spi_recv();
    
    // crc
    spi_send(0xff);
    spi_send(0xff);
    
    last_stage = SD_READ_OK;
    spi_deactivate();
    
    return true;
}

bool sdcard_write_page(uint32_t block)
{
    spi_activate();
    
    // send read command
    command(CMD24, block, 0);
    uint8_t r = spi_recv_ignore_ff();
    last_response = r;
    if (r != 0) {
        spi_deactivate();
        last_stage = SD_WRITE_REJECTED;
        return false;
    }
    
    // write data to card
    spi_send(0xfe);
    for (uint16_t i = 0; i < 512; ++i)
        spi_send(buffer[i]);
    
    // wait for a response
    uint8_t rr = 0;
    for (int i = 0; i < MAX_WRITE_ATTEMPTS; ++i) {
        rr = spi_recv();
        if (rr != 0xff)
            goto response_received;
        _delay_ms(10);
    }
    
    // response timeout
    spi_deactivate();
    last_stage = SD_WRITE_TIMEOUT;
    return false;

response_received:
    if ((rr & 0x1f) != 0x5) {
        spi_deactivate();
        last_response = rr;
        last_stage = SD_WRITE_DATA_REJECTED;
        return false;
    }
    
    // wait for write to finish
    for (int i = 0; i < MAX_WRITE_ATTEMPTS; ++i) {
        rr = spi_recv_ignore_ff();
        if (rr != 0x0)
            goto response_data_received;
        _delay_ms(10);
    }
    
    // response timeout
    spi_deactivate();
    last_stage = SD_WRITE_DATA_TIMEOUT;
    return false;

response_data_received:
    last_stage = SD_WRITE_OK;
    spi_deactivate();
    
    return true;
}

SDCardStage sdcard_last_stage()
{
    return last_stage;
}

uint8_t sdcard_last_response()
{
    return last_response;
}