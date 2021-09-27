PROJECT=fortuna-2
OBJECTS=main.o uart.o ram.o random.o z80.o post.o
AVRDUDE_FLAGS=-p atmega16 -C +./avrdude_gpio.conf -c pi_1 -V
FUSES=-U lfuse:w:0xff:m -U hfuse:w:0xd9:m    # external crystal
MCU=atmega16
F_CPU=16500000UL
BAUD=38400

CC=avr-gcc
CPPFLAGS=-std=c11 -Wall -Wextra -Os -DF_CPU=${F_CPU} -DBAUD=${BAUD} -mmcu=${MCU} -ffunction-sections -fdata-sections -mcall-prologues -I../common

all: ${PROJECT}.hex

${PROJECT}.hex: ${PROJECT}.elf
	avr-objcopy -j .text -j .data -O ihex $< $@
	arm-linux-gnueabihf-size ${PROJECT}.hex

${PROJECT}.elf: ${OBJECTS}
	$(CC) -mmcu=${MCU} -o $@ $^ -Wl,--gc-sections
	avr-size -C --mcu=${MCU} ${PROJECT}.elf	

test-connection:
	sudo avrdude ${AVRDUDE_FLAGS}

upload: ${PROJECT}.hex
	sudo avrdude ${AVRDUDE_FLAGS} -U flash:w:$<:i

fuse:
	sudo avrdude ${AVRDUDE_FLAGS} ${FUSES}

size: ${PROJECT}.elf
	avr-size -C --mcu=${MCU} ${PROJECT}.elf	

assembly: ${PROJECT}.elf
assembly: CFLAGS += -save-temps

talk:
	@echo "To exit, type ~."
	cu -l /dev/ttyAMA0 -s 38400

clean:
	rm -f *.elf *.o **/*.o *.hex z80/*.roma *.i *.s

# vim: set ts=8 sts=8 sw=8 noexpandtab:
