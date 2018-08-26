#include <avr/io.h>               // import definitions
 
int main (void) {
  DDRB = 1;                       // set DDRB.PB0 to output
  OCR0A = 62500;                  // set OCR0A to 62500. 2Hz toggle.
  TCCR0A = 1<<COM0A0;             // set timer to toggle PB0 on compare match
  TCCR0B = 1<<WGM02 | 1<<CS01;    // set CTC mode | enable and set prescaler :8
  while(1);                       // wait forever
}
