/*
 * MagSpoof - "wireless" magnetic stripe/credit card emulator
 *
 * by Samy Kamkar
 *
 * http://samy.pl/magspoof/
 *
 * - Allows you to store all of your credit cards and magstripes in one device
 * - Works on traditional magstripe readers wirelessly (no NFC/RFID required)
 * - Can disable Chip-and-PIN (code not included)
 * - Correctly predicts Amex credit card numbers + expirations from previous card number (code not included)
 * - Supports all three magnetic stripe tracks, and even supports Track 1+2 simultaneously
 * - Easy to build using Arduino or ATtiny
 *
 *----------------------------------------------------------
 *
 * MagSpoofPI - code integration by Salvador Mendoza
 *
 * Main idea: be able to compile MagSpoof with variable tracks, and use MagSpoof
 * without arduino dependency, and be able to use it without removing it from 
 * the raspberry pi gpio.
 * Details and configuration example: https://netxing.wordpress.com/2016/08/27/magspoofpi/
 *-
 * Special code for Raspberry Pi implementing avr-gcc without arduino.
 * -
 * Main modifications from the original MagSpoof in functions like digitalWrite(), 
 * setup() and loop().
 *
 * Example of configuration:
 *rbpiPin - Resistor - attiny85
 *  19  ->  1k  -->  PB0
 *  21  ->  1k  -->  PB1
 *  23  ->  1k  -->  PB2
 * DC17 ->  n/a -->  VCC
 * GND20 -> n/a -->  GND
 *  15  --> 1k  -->  PB5
 * Any other pin to enable/disable MagSpoof after be compiled
 * That pin has to be True before "make install" GPIO.setup(pin, 1)
 * pin --> 10k -->     PB2
 * GND > to same 10k > PB2
 */


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#define interrupts() sei()
#define noInterrupts() cli()
#define PIN_A PB0
#define PIN_B PB1
#define BUTTON_PIN PB4
#define ENABLE_PIN PB3 // also green LED
#define SWAP_PIN PB2 // unused
#define CLOCK_US 200
#define HIGH 1
#define LOW 0
#define _BV(bit) (1 << (bit))

#define BETWEEN_ZERO 53 // 53 zeros between track1 & 2

#define TRACKS 2

// consts get stored in flash as we don't adjust them
const char* tracks[] = {
"%4059557240058686^22111015312020010591?;4059557240058686^22111015312020010591?\0", // Track 1
";4059557240058686=22111015312020010591?\0" // Track 2
};

char revTrack[41];
const int sublen[] = { 
  32, 48, 48 };

const int bitlen[] = { 
  7, 5, 5 };

unsigned int curTrack = 0;

void digitalWrite(int a, int b){
    if (b == 1) {
        PORTB |= (1<<a); // high
    }else if (b == 0){
        PORTB &= ~(1<<a); // low
    }else {
        PORTB &= ~(b<<a); // Anything else
    }
}
void blink(int pin, int times)
{
  int i;
  for (i = 0; i < times; i++)
  {
    //PORTB = 0b00001000; 
    digitalWrite(pin, HIGH);
    _delay_ms(200);
    digitalWrite(pin, LOW);
    _delay_ms(200);
    //PORTB = 0b00000000; 
  }
}
// stores track for reverse usage later
void storeRevTrack(int track)
{
  int i, j, tmp, crc, lrc = 0;
  track--; // index 0
  int dir = 0;

  for (i = 0; tracks[track][i] != '\0'; i++)
  {
    crc = 1;
    tmp = tracks[track][i] - sublen[track];

    for (j = 0; j < bitlen[track]-1; j++)
    {
      crc ^= tmp & 1;
      lrc ^= (tmp & 1) << j;
      tmp & 1 ?
        (revTrack[i] |= 1 << j) :
        (revTrack[i] &= ~(1 << j));
      tmp >>= 1;
    }
    crc ?
      (revTrack[i] |= 1 << 4) :
      (revTrack[i] &= ~(1 << 4));
  } 

  // finish calculating and send last "byte" (LRC)
  tmp = lrc;
  crc = 1;
  for (j = 0; j < bitlen[track]-1; j++)
  {
    crc ^= tmp & 1;
    tmp & 1 ?
      (revTrack[i] |= 1 << j) :
      (revTrack[i] &= ~(1 << j));
    tmp >>= 1;
  }
  crc ?
    (revTrack[i] |= 1 << 4) :
    (revTrack[i] &= ~(1 << 4));

  i++;
  revTrack[i] = '\0';
}
void setup()
{
  //DDRB = 0b00001111;
  DDRB |= (1 << PIN_A); // output
  DDRB |= (1 << PIN_B); // output
  DDRB |= (1 << ENABLE_PIN); // output
  DDRB &=~(1 << BUTTON_PIN); // input
  PORTB |= (1<<BUTTON_PIN); // high to use it like INPUT_PULLUP

  // blink to show we started up
  blink(ENABLE_PIN, 3);
  //store reverse track 2 to play later
  storeRevTrack(2);
}


// send a single bit out
int dir;
void playBit(int sendBit)
{
  dir ^= 1;
  digitalWrite(PIN_A, dir);
  digitalWrite(PIN_B, !dir);
  _delay_us(CLOCK_US);

  if (sendBit)
  {
    dir ^= 1;
    digitalWrite(PIN_A, dir);
    digitalWrite(PIN_B, !dir);
  }
  _delay_us(CLOCK_US);

}

// when reversing
void reverseTrack(int track)
{
  int i = 0;
  track--; // index 0
  dir = 0;
  int j;
  while (revTrack[i++] != '\0');
  i--;
  while (i--)
    for (j = bitlen[track]-1; j >= 0; j--)
      playBit((revTrack[i] >> j) & 1);
}

// plays out a full track, calculating CRCs and LRC
void playTrack(int track)
{
  int tmp, crc, lrc = 0;
  track--; // index 0
  dir = 0;

  // enable H-bridge and LED
  digitalWrite(ENABLE_PIN, HIGH);

  // First put out a bunch of leading zeros.
  int i, j;
  for (i = 0; i < 25; i++)
    playBit(0);

  // Check for the end of the track
  for (i = 0; tracks[track][i] != '\0'; i++)
  {
    crc = 1;
    tmp = tracks[track][i] - sublen[track];

    for (j = 0; j < bitlen[track]-1; j++)
    {
      crc ^= tmp & 1;
      lrc ^= (tmp & 1) << j;
      playBit(tmp & 1);
      tmp >>= 1;
    }
    playBit(crc);
  }  

  // finish calculating and send last "byte" (LRC)
  tmp = lrc;
  crc = 1;
  for (j = 0; j < bitlen[track]-1; j++)
  {
    crc ^= tmp & 1;
    playBit(tmp & 1);
    tmp >>= 1;
  }
  playBit(crc);

  // if track 1, play 2nd track in reverse (like swiping back?)
  if (track == 0)
  {
    // if track 1, also play track 2 in reverse
    // zeros in between
    for (i = 0; i < BETWEEN_ZERO; i++)
      playBit(0);

    // send second track in reverse
    reverseTrack(2);
  }

  // finish with 0's
  for (i = 0; i < 5 * 5; i++)
    playBit(0);

  digitalWrite(PIN_A, LOW);
  digitalWrite(PIN_B, LOW);
  digitalWrite(ENABLE_PIN, LOW);
}


void sleep()
{
  GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
  PCMSK |= _BV(PCINT2);                   // Use PB3 as interrupt pin
  ADCSRA &= ~_BV(ADEN);                   // ADC off
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement

  MCUCR &= ~_BV(ISC01);
  MCUCR &= ~_BV(ISC00);                   // Interrupt on rising edge
  sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
  sei();                                  // Enable interrupts
  sleep_cpu();                            // sleep

  cli();                                  // Disable interrupts
  PCMSK &= ~_BV(PCINT2);                  // Turn off PB3 as interrupt pin
  sleep_disable();                        // Clear SE bit
  ADCSRA |= _BV(ADEN);                    // ADC on

  sei();                                  // Enable interrupts
}

// XXX move playtrack in here?
ISR(PCINT0_vect) {
  /*  noInterrupts();
   while (digitalRead(BUTTON_PIN) == LOW);
   
   delay(50);
   while (digitalRead(BUTTON_PIN) == LOW);
   playTrack(1 + (curTrack++ % 2)); 
   delay(400);
   
   interrupts();*/

}


void loop()
{
  sleep();
  noInterrupts();
  while ((PINB & (1<<BUTTON_PIN)) != 0);
  _delay_ms(50);
  while ((PINB & (1<<BUTTON_PIN)) != 0);
  playTrack(1 + (curTrack++ % 2)); 
  _delay_ms(400);

  interrupts();
  //playTrack(1 + (curTrack++ % 2));
}
int main(void){
    setup();
    while(1){
        loop();
    }
    return 1;
}
