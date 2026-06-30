#ifndef TIMER_INTERNAL_H
#define TIMER_INTERNAL_H

#include <stdint.h>

typedef struct TIMER_8BIT_Register_s {
    volatile uint8_t TCCRA;
    volatile uint8_t TCCRB;
    volatile uint8_t TCNT;
    volatile uint8_t OCRA;
    volatile uint8_t OCRB;
}TIMER_8BIT_Register_t;

typedef struct TIMER_16BIT_Register_s {
    volatile uint8_t TCCRA;
    volatile uint8_t TCCRB;
    volatile uint8_t TCCRC;
    volatile uint8_t Reserved;
    volatile uint16_t TCNT;
    volatile uint16_t ICR;
    volatile uint16_t OCRA;
    volatile uint16_t OCRB;
}TIMER_16BIT_Register_t;


#define TIMER0_REGISTER_BASE_ADDR (0x44)
#define TIMER0 ((TIMER_8BIT_Register_t*)TIMER0_REGISTER_BASE_ADDR)

#define TIMER1_REGISTER_BASE_ADDR (0x80)
#define TIMER1 ((TIMER_16BIT_Register_t*)TIMER1_REGISTER_BASE_ADDR)

#define TIMER2_REGISTER_BASE_ADDR (0xB0)
#define TIMER2 ((TIMER_8BIT_Register_t*)TIMER2_REGISTER_BASE_ADDR)

#endif