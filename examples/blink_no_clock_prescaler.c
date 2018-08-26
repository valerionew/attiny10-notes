#include <avr/io.h>               // import definitions
#include <avr/interrupt.h>        // import cli()

int main (void) {
  char oldsreg = SREG;            // save the interrupt setting register to oldsreg
  cli();                          // disable all the interrupts (SREG = 0)
  CCP = 0xD8;                     // signature to CCP
  CLKMSR = 0;                     // use clock 00: Calibrated Internal 8 MHzOscillator
  CCP = 0xD8;                     // signature
  CLKPSR = 0;                     // set prescaler to :1 (0x00)
  SREG = oldsreg;                 // restore the sreg, enabling the interrupts

  DDRB = 1;                       // set DDRB.PB0 to output
  OCR0A = 7812;                   // set OCR0A to 7812. 2Hz toggle.
  TCCR0A = 1<<COM0A0;             // set timer to toggle PB0 on compare match
  TCCR0B = 1<<WGM02 | 3<<CS00;    // set CTC mode | enable and set prescaler :64
  while(1);                       // wait forever
}
