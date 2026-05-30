#include <Arduino.h>

// Define Password Sequence (indexes corresponding to button pins 0, 1, 2, 3)
const int password[] = {0, 1, 2, 3};
const int seqLength = 4;
int userInput[seqLength];
int currentIndex = 0;

// Bitmask for our 4 buttons on Port D: PD2, PD3, PD4, PD5
// (1<<2) | (1<<3) | (1<<4) | (1<<5) -> 0b00111100
const uint8_t BUTTON_MASK = (1 << DDD2) | (1 << DDD3) | (1 << DDD4) | (1 << DDD5);

void setup() {
  // 1. Configure Buttons (PD2-PD5) as INPUT with INTERNAL PULLUPS
  DDRD &= ~BUTTON_MASK;   // Clear bits to set as input
  PORTD |= BUTTON_MASK;  // Set bits HIGH to enable pull-ups

  // 2. Configure LEDs (PB0 and PB1) as OUTPUT
  DDRB |= (1 << DDB0) | (1 << DDB1); // Set bits to 1 for output

  // 3. Ensure LEDs start turned OFF
  PORTB &= ~((1 << PORTB0) | (1 << PORTB1));
}

void loop() {
  // Array of the specific pin bit numbers on Port D to easily loop through them
  const uint8_t pinBits[] = {PIND2, PIND3, PIND4, PIND5};

  for (int i = 0; i < 4; i++) {
    // Read PIND and check if the specific button bit is LOW (0)
    if (!(PIND & (1 << pinBits[i]))) { 
      
      _delay_ms(50); // Software debounce
      
      // Check again if it's still pressed
      if (!(PIND & (1 << pinBits[i]))) {
        userInput[currentIndex] = i;
        currentIndex++;
        
        // Wait for button release (loop while the bit remains 0)
        while (!(PIND & (1 << pinBits[i])));
        
        // If the max sequence length is reached, evaluate the attempt
        if (currentIndex == seqLength) {
          checkPassword();
          currentIndex = 0; // Reset index for next attempt
        }
      }
    }
  }
}

void checkPassword() {
  bool correct = true;
  for (int i = 0; i < seqLength; i++) {
    if (userInput[i] != password[i]) {
      correct = false;
      break;
    }
  }

  if (correct) {
    PORTB |= (1 << PORTB0);  // Green LED (PB0) HIGH
    _delay_ms(2000);
    PORTB &= ~(1 << PORTB0); // Green LED LOW
  } else {
    PORTB |= (1 << PORTB1);  // Red LED (PB1) HIGH
    _delay_ms(2000);
    PORTB &= ~(1 << PORTB1); // Red LED LOW
  }
}