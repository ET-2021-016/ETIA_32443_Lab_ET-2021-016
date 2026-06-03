#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

volatile bool em_on = false; // emergency
volatile uint32_t em_start = 0;// emergency start time
volatile bool ped_req = false; //pederastion request
volatile bool maint_on = false;// maintance 
volatile uint32_t ms = 0; // timer

void run_em();
void run_maint();
void run_ped();
void run_normal();
void wait(uint32_t t);
void check_em();

int main(void) {
    DDRB |= 0x1F; // Set PB0 to PB4 as outputs
    DDRD &= ~((1 << DDD2) | (1 << DDD3)); // Inputs: PD2, PD3
    DDRB &= ~(1 << DDB5); // Input: PB5


//The System Clock...................................................................
    TCCR1A = 0; 
    TCCR1B = (1 << WGM12) | (1 << CS11); 
    OCR1A = 1999; 
    TIMSK1 |= (1 << OCIE1A); 
//...................................................................................

//Interrupt Configuration...........................................................
    maint_on = !(PINB & (1 << PINB5));

    EICRA |= (1 << ISC01) | (1 << ISC11);
    EIMSK |= (1 << INT0) | (1 << INT1); 
    PCICR |= (1 << PCIE0);     
    PCMSK0 |= (1 << PCINT5);   

    sei(); 
//....................................................................................

//emergency loop (The Main Control Loop & Priority Logic).............................
    while (1) {
        check_em();
        if (em_on)        run_em();
        else if (maint_on) run_maint();
        else if (ped_req) { run_ped(); ped_req = false; }
        else               run_normal();
    }
    return 0;
}
//......................................................................................


//Interrupt Service Routines (ISRs).....................................................
ISR(TIMER1_COMPA_vect) { ms++; }

ISR(INT0_vect) {
    if (!em_on) { em_on = true; em_start = ms; }
}

ISR(INT1_vect) { if (!em_on) ped_req = true; }

ISR(PCINT0_vect) { maint_on = !(PINB & (1 << PINB5)); }

void check_em() {
    if (em_on && (ms - em_start >= 10000)) em_on = false;
}
//......................................................................................

//Non-Blocking Responsive Delay (wait)..................................................
void wait(uint32_t t) {
    uint32_t start = ms;
    while ((ms - start) < t) {
        check_em();
        if (em_on || (maint_on && !em_on)) break;
        asm volatile("nop"); 
    }
}
//.......................................................................................


//emergency stop
void run_em() {
    PORTB = (1 << PORTB0) | (1 << PORTB4);
}

//maintance
void run_maint() {
    PORTB &= ~0x1D; 
    PORTB |= (1 << PORTB1);  wait(500);
    PORTB &= ~(1 << PORTB1); wait(500);
}

//pedestrain crossing
void run_ped() {
    if (PORTB & (1 << PORTB0)) {
        PORTB &= ~(1 << PORTB0); PORTB |= (1 << PORTB1);  wait(2000);
        PORTB &= ~0x12;          PORTB |= 0x0C;           wait(5000);
    }
}

void run_normal() {
    PORTB = 0b10001;
    wait(5000);
    if (em_on || maint_on || ped_req) return;
    PORTB = 0b1100; 
    wait(5000);
}