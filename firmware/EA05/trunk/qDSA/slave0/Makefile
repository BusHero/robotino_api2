TARGET            = slave
OBJ            = main.o spislave.o motor.o pid.o ../share/src/adc.o ../share/src/watchdog.o
MCU_TARGET     = atmega8535
OPTIMIZE       = -O1

FLAGS	= -I../share/include -I../../share/include -I../../../share/include
DEFS           = -DF_CPU=8
LIBS           =

ifneq ($(PID_DISABLED),)
DEFS += -DPID_DISABLED
TARGET = slave_pid_disabled
endif

# You should not have to change anything below here.

CC            = avr-gcc
AVRDUDE = avrdude
AVRDUDE_PROGRAMMER = stk500
AVRDUDE_PORT = $(AVRCOM)	   # programmer connected to serial device

AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).srec

AVRDUDE_FLAGS = -p $(MCU_TARGET) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)

# Override is only needed by avr-lib build system.

override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS) $(FLAGS)
override LDFLAGS       = -Wl,-Map,$(TARGET).map

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all: $(TARGET).elf lst text eeprom

internal8mhz:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -u -U lfuse:w:0xe4:m
	
up: $(TARGET).srec
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)


#	$(UISPBIN) -dprog=stk500 -dpart=ATmega8535 -dserial=$(SERIALDEVICE) -dspeed=115200 --upload --erase --verify if=$(TARGET).srec

$(TARGET).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o $(TARGET).elf $(TARGET).bin $(TARGET).hex $(TARGET).lst $(TARGET).map $(TARGET).srec

lst:  $(TARGET).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

# Rules for building the .text rom images

text: hex bin srec

hex:  $(TARGET).hex
bin:  $(TARGET).bin
srec: $(TARGET).srec

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@
	cp $(TARGET).hex ../bin

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@
#cp $(TARGET).srec ../bin
	
%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

# Rules for building the .eeprom rom images

eeprom: ehex ebin esrec

ehex:  $(TARGET)_eeprom.hex
ebin:  $(TARGET)_eeprom.bin
esrec: $(TARGET)_eeprom.srec