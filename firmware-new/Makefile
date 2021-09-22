PROJECT=fortuna-2
OBJECTS=main.o
AVRDUDE_FLAGS=-p atmega16 -C +./avrdude_gpio.conf -c pi_1 -V
FUSES=-U lfuse:w:0xff:m -U hfuse:w:0xd9:m    # external crystal
MCU=atmega16
F_CPU=16500000UL

CC=avr-gcc
CPPFLAGS=-std=c11 -Wall -Wextra -Os -DF_CPU=${F_CPU} -mmcu=${MCU} -ffunction-sections -fdata-sections -mcall-prologues -I../common
# CPPFLAGS+=-DRUN_TESTS  # execute tests on initialization
# CPPFLAGS+=-DDEBUG      # print debugging information during execution
# CPPFLAGS+=-DSTEP       # wait for a keypress before each step

all: ${PROJECT}.hex

${PROJECT}.hex: ${PROJECT}.elf
	avr-objcopy -j .text -j .data -O ihex $< $@
	arm-linux-gnueabihf-size ${PROJECT}.hex

${PROJECT}.elf: ${OBJECTS}
	$(CC) -mmcu=${MCU} -o $@ $^ -Wl,--gc-sections
	avr-size -C --mcu=${MCU} ${PROJECT}.elf	

test-connection:
	sudo avrdude ${AVRDUDE_FLAGS}

clean:
	rm -f boot.tmp boot.c *.elf *.o *.hex z80/*.rom

upload: ${PROJECT}.hex
	sudo avrdude ${AVRDUDE_FLAGS} -U flash:w:$<:i

fuse:
	sudo avrdude ${AVRDUDE_FLAGS} ${FUSES}

size: ${PROJECT}.elf
	avr-size -C --mcu=${MCU} ${PROJECT}.elf	

talk:
	@echo "To exit, type ~."
	cu -l /dev/ttyAMA0 -s 38400

# vim: set ts=8 sts=8 sw=8 noexpandtab:
