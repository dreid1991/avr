
PRG = hello

all: $(PRG).c
	avr-gcc $(PRG).c -mmcu=attiny85 -O -o $(PRG).o 
	avr-objcopy -O ihex $(PRG).o $(PRG).hex
	avrdude -c usbtiny -p t85 -P usb -U flash:w:$(PRG).hex

build: $(PRG).c
	avr-gcc $(PRG).c -mmcu=attiny85 -O -o $(PRG).o 



flash: $(PRG).c
	avr-objcopy -O ihex $(PRG).o $(PRG).hex
	avrdude -c usbtiny -p t85 -P usb -U flash:w:$(PRG).hex

