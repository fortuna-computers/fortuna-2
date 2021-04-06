#include "sdcard.hh"

#include <util/delay.h>

#include "../libf1comm/fortuna1def.hh"

#define CMD0   0
#define CMD8   8
#define CMD17  17
#define CMD24  24
#define CMD55  55
#define CMD58  58
#define ACMD41 41

#define MAX_READ_ATTEMPTS 20
#define MAX_WRITE_ATTEMPTS 100

SDCard::SDCard(SPI& spi, RAM& ram)
    : spi_(spi), ram_(ram)
{
    spi_.deactivate();
}

bool SDCard::initialize()
{
    // will attempt to initialize 10 times
    for (size_t i = 0; i < 10; ++i) {
        if (initialize_once())
            return true;
        _delay_ms(80);
    }
    return false;
}

bool SDCard::initialize_once()
{
    ram_.write_byte(SD_STATUS, 0b11);  // start as initialization error
    last_response_ = 0xff;
    last_stage_ = SD_RESET;
    reset();
    
    last_stage_ = SD_GO_IDLE;
    last_response_ = go_idle();
    if (last_response_ != 0x1)
        return false;
   
    uint32_t response = 0;
    last_stage_ = SD_IF_COND;
    last_response_ = if_cond(&response);
    if (last_response_ > 1)
        return false;
    
    last_stage_ = SD_INIT;
    for (int i = 0; i < 16; ++i) {
        last_response_ = init_process(&response);
        if (last_response_ == 0) {
            ram_.write_byte(SD_STATUS, 0b10);  // initialization success
            return true;
        }
        _delay_ms(50);
    }
    return false;
}

bool SDCard::read_page(uint32_t block, Buffer& buffer)
{
    ram_.write_byte(SD_STATUS, 0b101);  // start as read error
    
    spi_.activate(SPI::SD);
    
    // send read command
    command(CMD17, block, 0);
    uint8_t r = spi_.recv_ignore_ff();
    last_response_ = r;
    if (r != 0) {
        spi_.deactivate();
        last_stage_ = SD_READ_REJECTED;
        return false;
    }

    // read data
    uint8_t rr = 0;
    for (int i = 0; i < MAX_READ_ATTEMPTS; ++i) {
        rr = spi_.recv_ignore_ff();
        if (rr == 0xfe)
            goto read_data;
        _delay_ms(10);
    }

    // read timeout
    spi_.deactivate();
    last_stage_ = SD_READ_TIMEOUT;
    return false;

read_data:
    for (int i = 0; i < 512; ++i)
        buffer.data[i] = spi_.recv();
    buffer.sz = 512;

    // crc
    spi_.send(0xff);
    spi_.send(0xff);

    last_stage_ = SD_READ_OK;
    spi_.deactivate();
    
    ram_.write_byte(SD_STATUS, 0b100);  // read success
    
    return true;
}

bool SDCard::write_page(uint32_t block, Buffer const& buffer)
{
    ram_.write_byte(SD_STATUS, 0b1001);  // start as write error
    
    spi_.activate(SPI::SD);
    
    // send read command
    command(CMD24, block, 0);
    uint8_t r = spi_.recv_ignore_ff();
    last_response_ = r;
    if (r != 0) {
        spi_.deactivate();
        last_stage_ = SD_WRITE_REJECTED;
        return false;
    }

    // write data to card
    spi_.send(0xfe);
    for (uint16_t i = 0; i < 512; ++i)
        spi_.send(buffer.data[i]);

    // wait for a response
    uint8_t rr = 0;
    for (int i = 0; i < MAX_WRITE_ATTEMPTS; ++i) {
        rr = spi_.recv();
        if (rr != 0xff)
            goto response_received;
        _delay_ms(10);
    }

    // response timeout
    spi_.deactivate();
    last_stage_ = SD_WRITE_TIMEOUT;
    return false;

response_received:
    if ((rr & 0x1f) != 0x5) {
        spi_.deactivate();
        last_response_ = rr;
        last_stage_ = SD_WRITE_DATA_REJECTED;
        return false;
    }

    // wait for write to finish
    for (int i = 0; i < MAX_WRITE_ATTEMPTS; ++i) {
        rr = spi_.recv_ignore_ff();
        if (rr != 0x0)
            goto response_data_received;
        _delay_ms(10);
    }

    // response timeout
    spi_.deactivate();
    last_stage_ = SD_WRITE_DATA_TIMEOUT;
    return false;

response_data_received:
    last_stage_ = SD_WRITE_OK;
    spi_.deactivate();
    
    ram_.write_byte(SD_STATUS, 0b1000);  // write success
    
    return true;
}

void SDCard::reset()
{
    // powerup card
    spi_.deactivate();
    _delay_ms(1);
    for (uint8_t i = 0; i < 10; ++i)
        spi_.send(0xff);
    
    // deselect card
    spi_.deactivate();
    spi_.send(0xff);
}

uint8_t SDCard::go_idle()
{
    spi_.activate(SPI::SD);
    command(CMD0, 0, 0x94);
    uint8_t r1 = spi_.recv_ignore_ff();
    spi_.deactivate();
    return r1;
}

uint8_t SDCard::if_cond(uint32_t* response)
{
    spi_.activate(SPI::SD);
    command(CMD8, 0x1AA, 0x86);
    uint8_t r1 = spi_.recv_ignore_ff();
    if (r1 <= 1) {
        *response = 0;
        *response |= (uint32_t) spi_.send(0xff) << 24;
        *response |= (uint32_t) spi_.send(0xff) << 16;
        *response |= (uint32_t) spi_.send(0xff) << 8;
        *response |= (uint32_t) spi_.send(0xff);
    }
    spi_.deactivate();
    return r1;
}

uint8_t SDCard::init_process(uint32_t* response)
{
    spi_.activate(SPI::SD);
    command(CMD55, 0, 0);
    uint8_t r1 = spi_.recv_ignore_ff();
    spi_.deactivate();
    if (r1 > 1)
        return r1;
    
    spi_.activate(SPI::SD);
    command(ACMD41, 0x40000000, 0);
    r1 = spi_.recv_ignore_ff();
    if (r1 <= 1) {
        *response = 0;
        *response |= (uint32_t) spi_.send(0xff) << 24;
        *response |= (uint32_t) spi_.send(0xff) << 16;
        *response |= (uint32_t) spi_.send(0xff) << 8;
        *response |= (uint32_t) spi_.send(0xff);
    }
    spi_.deactivate();
    
    return r1;
}

void SDCard::command(uint8_t cmd, uint32_t args, uint8_t crc)
{
    spi_.send(cmd | 0x40);
    spi_.send((uint8_t)(args >> 24));
    spi_.send((uint8_t)(args >> 16));
    spi_.send((uint8_t)(args >> 8));
    spi_.send((uint8_t)args);
    spi_.send(crc | 0x1);
}
