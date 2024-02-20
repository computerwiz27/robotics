#ifndef _TIMER_3_H
#define _TIMER_3_H

// Using datasheet for ATmega32U4 
// https://www.microchip.com/en-us/product/ATmega32u4

void setupTimer3() {

    // disable interrupts on output compare match A
    // page 138, 14.10.18
    TIMSK3 &= ~(1 << OCIE3A);

    // reset timer3 controll registers
    // pages 131 - 135, 14.10.2 - 14.10.6
    TCCR3A = 0;
    TCCR3B = 0;
    TCCR3C = 0;

    // set prescaler to 64
    // page 134, 14.10.4
    TCCR3B |= (1 << CS30);
    TCCR3B |= (1 << CS31);
    TCCR3B &= ~(1 << CS32);

    // clear timer3 on compare
    // page 133, 14.10.3
    TCCR3B |= (1 << WGM32);

    // load value to match against in reg A
    // page 136, 14.10.12
    int count_to = (int)(MEASURE_TS / (62.5 * pow(10, -6) * 64));
    OCR3A = count_to;

    // reset timer3
    TCNT3 = 0;

    // enable interrupts on output compare match A
    // page 138, 14.10.18
    TIMSK3 |= (1 << OCIE3A);
}

#endif