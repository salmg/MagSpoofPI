
Original instructions to program Attiny85 from Raspberry Pi: 
http://www.instructables.com/id/Programming-the-ATtiny85-from-Raspberry-Pi/

Dependencies:

Download and build avrdude

sudo apt-get install bison automake autoconf flex git gcc
sudo apt-get install gcc-avr binutils-avr avr-libc
git clone https://github.com/kcuzner/avrdude 
cd avrdude/avrdude
./bootstrap && ./configure && sudo make install

Setup SPI on the GPIO

sudo raspi-config
and Enable SPI device in the Advanced Options (see picture)
You can check this at the command line with lsmod, no need to reboot. (Maybe need to sudo modprobe spidev)

Download and build WiringPi for the gpio commands

cd ~
git clone git://git.drogon.net/wiringPi
cd wiringPi
./build


MagSpoofPI code integration by Salvador Mendoza
Special code for Raspberry Pi implementing avr-gcc without arduino.
Main modification in some functions, like digitalWrite(), setup() and loop()

 * Example of configuration:
 rbpi - Resistor - attiny85
 19  -->  1k  -->  PB0
 21  -->  1k  -->  PB1
 23  -->  1k  -->  PB2
DC17 --> n/a  -->  VCC
GND20 -> n/a  -->  GND
 15  -->  1k  -->  PB5
Any other pin to enable/disable MagSpoof after be compiled
That pin has to be True before "make install" GPIO.setup(pin, 1)
