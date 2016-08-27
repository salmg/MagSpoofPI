MagSpoofPI - code integration by Salvador Mendoza(salvador_m_g@msn.com) @Netxing <br>
Thanks to Samy Kamkar(@samykamkar) and Sabas(@Sabasacustico)<br>

Original schema for MagSpoof:

<img class="  wp-image-583 aligncenter" src="https://netxing.files.wordpress.com/2016/08/magspoof-schematic-dip.png" alt="magspoof-schematic-dip" width="492" height="273" />

In the MagSpoofPI version, the connections to the L293D are the same as the original. The only main changes are in the attiny85 side. I will use Raspberry pi pin numbers, not GPIO numbers to describe this article:

We will need four 1k resistors, connecting the Raspberry pins: 19(MOSI) to the 1K resistor and then to the Attiny's PB0. Following the same sequence MISO and SCK. In this example, I used the Raspberry pin 22 as reset button, connecting to the 1k resistor and then to the Attiny's PB5.

Skip this step if you are not connecting the control(button) pin during the compilation process. If you are, the pin # 29 that I used it as button to enable/disable the MagSpoof after it is compiled must set to 1 before building the code: GPIO.output(29, 1) to avoid errors and conflicts with SCK(PB2).

The pin 29 is connected to a 10K resistor. The other end of the resistor to GND.

Example of this configuration:

[caption id="attachment_629" align="aligncenter" width="529"]<img class="alignnone  wp-image-629" src="https://netxing.files.wordpress.com/2016/08/screen-shot-2016-08-26-at-7-46-24-pm.png" alt="MagSpoofPI Example" width="529" height="672" /> MagSpoofPI Example[/caption]

&nbsp;

Sal.

Main idea: be able to compile MagSpoof with variable tracks, and use MagSpoof<br>
without arduino dependencies, and without removing it from the raspberry pi gpio.<br>
-<br>
Special code for Raspberry Pi implementing avr-gcc without arduino.<br>
-<br>
Main modifications from the original MagSpoof in functions like digitalWrite(), <br>
setup() and loop().<br>
Special code for Raspberry Pi implementing avr-gcc.<br>

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
