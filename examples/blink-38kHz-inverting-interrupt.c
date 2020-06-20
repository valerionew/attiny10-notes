/*
Valerio Nappi (valerionew) https://github.com/valerionew/attiny10-notes

This software produces a square wave at circa 38kHz (can be tuned via the preloaded
value into OCR0A) and inverts the signal on PB2 (pin change interrupt) to PB1
It was designed to drive an IR led and a demodulating receiver, also inverting the
demod output, as part as a soldering iron holder sensing device, developed for
the unisolder open source soldering ironn (https://github.com/valerionew/unisolder-notes)

------------------------------------WARNING------------------------------------
WARNING: the system clock prescaler change needs the -Os optimization compiling
optimization. Otherwise the signature loading into CPP won't be timely (4 clocks)
See:
https://github.com/valerionew/attiny10-notes#setting-the-prescaler
for reference
------------------------------------WARNING------------------------------------
*/


/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/


#include <avr/interrupt.h> // for cli

int main (void) {
  char oldsreg = SREG;            // save the interrupt setting register to oldsreg
  cli();                          // disable all the interrupts (SREG = 0)
  CCP = 0xD8;                     // signature to CCP
  CLKMSR = 0;                     // use clock 00: Calibrated Internal 8 MHzOscillator
  CCP = 0xD8;                     // signature
  CLKPSR = 0;                     // set prescaler to :1 (0x00)
  SREG = oldsreg;                 // restore the sreg, enabling the interrupts

  DDRB = 1 << PB0;                // set DDRB.PB0 to output
  OCR0A = 105;                    // set OCR0A
  TCCR0A = 1<<COM0A0;             // set timer to toggle PB0 on compare match
  TCCR0B = 1<<WGM02 | 1<<CS00;    // set CTC mode | enable and set prescaler :1

  DDRB |= 1 << PB1;               // set PB1 as output
  EICRA = 1 << ISC00;             // set interrupt on pin change
  EIMSK = 1 << INT0;              // set the ext int mask
  SREG = 1 << 7;                  // enable global interrupts
  while(1);
}


ISR(INT0_vect){
 //PORTB = (PORTB & ~(1<<PB1)) | ((~PINB & (1<<PB2))>>1); ??
if ((~PINB) & (1<<PB2))
  PORTB |= 1<<PB1;
else
  PORTB &= ~(1<<PB1);
}
