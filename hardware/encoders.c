#include "encoders.h"

// Using datasheet for ATmega32U4 
// https://www.microchip.com/en-us/product/ATmega32u4

void PCINT0_vector() {
    boolean el_A = digitalRead(ENCODER_L_XOR_PIN);

    // read the input of PE2
    // page 85, 10.4.13
    boolean el_B = PINE & (1<<PINE2);

    el_A = el_A ^ el_B;

    // State :  (bit3)  (bit2)  (bit1)  (bit0)
    // State :  New B,  New A,  Old B,  Old A.
    state_e_l = state_e_l | (el_B << 3);
    state_e_l = state_e_l | (el_A << 2);

    if (state_e_l == 0b0001) {
        count_e_l--;
    } else if (state_e_l == 0b0010) {
        count_e_l++;
    } else if (state_e_l == 0b0100) {
        count_e_l++;
    } else if (state_e_l == 0b0111) {
        count_e_l--;
    } else if (state_e_l == 0b1000) {
        count_e_l--;
    } else if (state_e_l == 0b1011) {
        count_e_l++;
    } else if (state_e_l == 0b1101) {
        count_e_l++;
    } else if (state_e_l == 0b1110) {
        count_e_l--;
    }

    state_e_l = state_e_l >> 2;
}

void INT6_vector() {
    boolean er_A = digitalRead(ENCODER_R_XOR_PIN);
    boolean er_B = digitalRead(ENCODER_R_IN_PIN);
    er_A = er_A ^ er_B;

    // State: (bit3)  (bit2)  (bit1)   (bit0)
    // State:  new B   new A   old B   old A
    state_e_r = state_e_r | (er_B << 3);
    state_e_r = state_e_r | (er_A << 2);

    if (state_e_r == 0b0001) {
        count_e_r--;
    } else if (state_e_r == 0b0010) {
        count_e_r++;
    } else if (state_e_r == 0b0100) {
        count_e_r++;
    } else if (state_e_r == 0b0111) {
        count_e_r--;
    } else if (state_e_r == 0b1000) {
        count_e_r--;
    } else if (state_e_r == 0b1011) {
        count_e_r++;
    } else if (state_e_r == 0b1101) {
        count_e_r++;
    } else if (state_e_r == 0b1110) {
        count_e_r--;
    }

    state_e_r = state_e_r >> 2; 
}

void setupEncoderL() {
    count_e_l = 0;

    //a=10;

    // The state of the left encoder input is found at pin 
    // PE2 (port E, pin 2).

    // set PE2 as input
    // page 85, 10.4.12
    DDRE = DDRE & ~(1<<DDE6);
    //                   ^ sholdn't this be DDE2 ???

    // enable pull up resistor for PE2
    // page 85, 10.4.11
    PORTE = PORTE | (1 << PORTE2 );

    pinMode(ENCODER_L_XOR_PIN, INPUT);
    digitalWrite(ENCODER_L_XOR_PIN, HIGH);

    state_e_l = 0;

    boolean el_A = digitalRead(ENCODER_L_XOR_PIN); 

    // read the input of PE2
    // page 85, 10.4.13
    boolean el_B = PINE & (1<<PINE2);
    // maybe shift el_B >> 2 ?
    
    el_A = el_A ^ el_B;
    
    state_e_l = state_e_l | (el_B << 1);
    state_e_l = state_e_l | (el_A << 0);

    // disable interrupts for PCIE0
    // page 91, 11.1.5
    PCICR = PCICR & ~( 1 << PCIE0 );

    // set interrupt on PCINT0
    // page 91, 11.1.7
    PCMSK0 |= (1 << PCINT4);
    //              ^ there is a mismatch to PCINT4

    // clear the interrupt flag
    // page 91, 11.1.6
    PCIFR |= (1 << PCIF0);

    // enable interrupts for PCIE0
    PCICR |= (1 << PCIE0);
}

void setupEncoderR() {
    count_e_r = 0;

    pinMode(ENCODER_R_IN_PIN, INPUT);
    pinMode(ENCODER_R_XOR_PIN, INPUT);

    state_e_r = 0;

    boolean er_A = digitalRead(ENCODER_R_XOR_PIN);
    boolean er_B = digitalRead(ENCODER_R_IN_PIN);
    er_A = er_A ^ er_B;

    state_e_r = state_e_r | (er_B << 1);
    state_e_r = state_e_r | (er_A << 0);

    // disable external interrupts for INT6
    // page 90, 11.1.3
    EIMSK = EIMSK & ~(1<<INT6);

    // set up INT6 interrupt
    // page 89, 11.1.2
    EICRB |= ( 1 << ISC60 );

    // clear the interrupt flag
    //page 90, 11.1.4
    EIFR |= ( 1 << INTF6 );

    // enable external interrupts for INT6
    // page 90, 11.1.3
    EIMSK |= ( 1 << INT6 );
}

void setupEncoders() {
    setupEncoderL();
    setupEncoderR();
}