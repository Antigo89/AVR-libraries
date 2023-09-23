 
DEVICE=atmega328p
CLOCK_T=16000000
CLOCK_F=8000000
CLOCK=$(CLOCK_T)
HFUSE=0xD9
LFUSE=0xFC

ISP=usbasp
PORT=/dev/usb/ttyUSB0

OBJECTS	=	main.o

COMPILE	=	avr-gcc	-Wall	-Os	-DF_CPU=$(CLOCK)	-mmcu=$(DEVICE)

all:	main.hex

.c.o:
	$(COMPILE)	-c $<	-o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@

.c.s:
	$(COMPILE) -S $< -o $@

#flash:	all
#	$(AVRDUDE) -U flash:w:main.hex:i

clean:
	rm -f main.hex main.elf $(OBJECTS)

main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

#main.bin:	$(OBJECTS)
#	$(COMPILE) -o main.bin $(OBJECTS) -Wl,-Map,main.map
	
main.hex: main.elf
	rm -f main.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-size --format=avr --mcu=$(DEVICE) main.elf

flash:
	tools/avrdude -c$(ISP) -p$(DEVICE) -P$(PORT) -U flash:w:main.hex

fuses_final:
	tools/avrdude -c$(ISP) -p$(DEVICE) -P$(PORT) -u -U hfuse:w:$(HFUSE):m -U lfuse:w:$(LFUSE):m
	
fuses_read:
	tools/avrdude -c$(ISP) -p$(DEVICE) -P$(PORT) -U hfuse:r:hfuse.txt:h -U lfuse:r:lfuse.txt:h

avrdude:
	tools/avrdude -c$(ISP) -p$(DEVICE) -P$(PORT) -v
	
help:
	@echo "Usage: make                compile all"
	@echo "       make help           help"
	@echo "       make avrdude        testind connect"
	@echo "       make flash          upload in devices"
	@echo "       make fuses_read     upload in devices"
	@echo "       make fuses_final    write FUSES Byte (warning! only final devices)"
