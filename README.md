MagSpoofPI - code integration by Salvador Mendoza

Main idea: be able to compile MagSpoof with variable tracks, and use MagSpoof<br>
without arduino dependencies, and without removing it from the raspberry pi gpio.<br>
-<br>
Special code for Raspberry Pi implementing avr-gcc without arduino.<br>
-<br>
Main modifications from the original MagSpoof in functions like digitalWrite(), <br>
setup() and loop().

Special code for Raspberry Pi implementing avr-gcc without arduino.<br>
Main modification in some functions, like digitalWrite(), setup() and loop()<br>

Original MagSpoof: http://samy.pl/magspoof/

Original instructions to program Attiny85 from Raspberry Pi: <br>
http://www.instructables.com/id/Programming-the-ATtiny85-from-Raspberry-Pi/

Dependencies:
Download and build avrdude

sudo apt-get install bison automake autoconf flex git gcc<br>
sudo apt-get install gcc-avr binutils-avr avr-libc<br>
git clone https://github.com/kcuzner/avrdude <br>
cd avrdude/avrdude<br>
./bootstrap && ./configure && sudo make install<br>


Setup SPI on the GPIO

sudo raspi-config<br>
and Enable SPI device in the Advanced Options (see picture)<br>
You can check this at the command line with lsmod, no need to reboot. (Maybe need to sudo modprobe spidev)

Download and build WiringPi for the gpio commands

cd ~ <br>
git clone git://git.drogon.net/wiringPi<br>
cd wiringPi<br>
./build<br>

Example of configuration raspberry pi and attiny85:<br>
rbpiPin - Resistor - attiny85<br>
 19  -->  1k  -->  PB0<br>
 21  -->  1k  -->  PB1<br>
 23  -->  1k  -->  PB2<br>
DC17 --> n/a  -->  VCC<br>
GND20 -> n/a  -->  GND<br>
 15  -->  1k  -->  PB5<br>
Any other pin to enable/disable MagSpoof after be compiled<br>
That pin has to be True before "make install" GPIO.setup(pin, 1)<br>

make install
