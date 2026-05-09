#include <Arduino.h>

void setup()
{

  DDRD = 0b00000111;
  //DDRD &= ~(1 << PD3); 
  DDRB= B00000001;
}

void loop()
{
 PORTD = B00000001;
 PORTB= ~(1<<PB0);// Inverse of the output bit 
 delay(1000);
 PORTD= (1<<PD1);  
 delay(1000);
 PORTB= (1<<PB0);
 PORTD&= ~(1<<PD1);
 delay(1000); 
}