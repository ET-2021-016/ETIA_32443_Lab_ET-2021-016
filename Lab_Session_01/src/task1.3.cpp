#include <Arduino.h>

void setup()
{

  DDRD = 0b00000011;
  //DDRD &= ~(1 << PD3); 

  //DDRB = 0b00000000; 
  DDRB |= (1 << PB0);
}

void loop()
{
  
  if (PIND & (1 << PD3)) { 
    PORTB |= (1 << PB0); 
  } 
  else {
    PORTB &= ~(1 << PB0); 
}
}