
PRG = main
FLAGS = -std=c99 -O2
all: $(PRG).c
	avr-gcc $(PRG).c -mmcu=attiny85 $(FLAGS) -o $(PRG).o 
	avr-objcopy -O ihex $(PRG).o $(PRG).hex
	avrdude -c usbtiny -p t85 -P usb -U flash:w:$(PRG).hex

build: $(PRG).c
	avr-gcc $(PRG).c -mmcu=attiny85 $(FLAGS) -o $(PRG).o 



flash: $(PRG).c
	avr-objcopy -O2 ihex $(PRG).o $(PRG).hex
	avrdude -c usbtiny -p t85 -P usb -U flash:w:$(PRG).hex

test: $(PRG).c helpers.cpp globals.c
	g++ -O2 $(PRG).c -o test.out -std=c++11
