TARGET            = serial_echo
OBJ            = main.o ../../share/src/serial.o ../../share/src/led.o
MCU_TARGET     = atmega8535
OPTIMIZE       = -O1

FLAGS	= -I../../share/include
DEFS           = -DF_CPU=14.7456 -DLED_PORT=PORTD -DLED_PIN=PIND -DLED_DDR=DDRD -DLED_BIT=5
LIBS           =

# You should not have to change anything below here.

CC            = avr-gcc
AVRDUDE = avrdude
AVRDUDE_PROGRAMMER = stk500
AVRDUDE_PORT = com5	   # programmer connected to serial device

AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).srec
#AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(TARGET).eep

AVRDUDE_FLAGS = -p $(MCU_TARGET) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)

# Uncomment the following if you want avrdude's erase cycle counter.
# Note that this counter needs to be initialized first using -Yn,
# see avrdude manual.
#AVRDUDE_ERASE += -y

# Uncomment the following if you do /not/ wish a verification to be
# performed after programming the device.
#AVRDUDE_FLAGS += -V

# Increase verbosity level.  Please use this when submitting bug
# reports about avrdude. See <http://savannah.nongnu.org/projects/avrdude> 
# to submit bug reports.
#AVRDUDE_FLAGS += -v -v


# Override is only needed by avr-lib build system.

override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS) $(FLAGS)
override LDFLAGS       = -Wl,-Map,$(TARGET).map

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all: $(TARGET).elf lst text eeprom

external_crystal:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -u -U lfuse:w:0xef:m
	
#$(UISPBIN) -dprog=stk500 -dpart=$(MCU_TARGET) -dserial=$(SERIALDEVICE) -dspeed=115200 --wr_fuse_l=0xef

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

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

# Rules for building the .eeprom rom images

eeprom: ehex ebin esrec

ehex:  $(TARGET)_eeprom.hex
ebin:  $(TARGET)_eeprom.bin
esrec: $(TARGET)_eeprom.srec