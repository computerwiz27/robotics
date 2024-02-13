#ifndef _ENCODERS_H
#define _ENCODERS_H

//#define ENCODER_L_IN_PIN  PE2 
#define ENCODER_L_XOR_PIN   26
#define ENCODER_R_IN_PIN    23
#define ENCODER_R_XOR_PIN   7


volatile long count_e_l;
volatile byte state_e_l;
volatile long count_e_r;
volatile byte state_e_r;

// Left encoder
ISR(PCINT0_vect);

// Right encoder
ISR(INT6_vect);

void setupEncoders();


#endif
