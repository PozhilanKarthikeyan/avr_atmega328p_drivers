#ifndef _GPIO_HELP_H
#define _GPIO_HELP_H

#include <stdint.h>

typedef struct GPIO_Register_s {
    uint8_t PIN;
    uint8_t DDR;
    uint8_t PORT;
}GPIO_Register_t;

#define PORT_REGISTER_BASE_ADDR (0x23)
#define PCIMSK_REGISTER_BASE_ADDR (0X6B)

#define PORT_REGISTER_BASE_PTR ((volatile GPIO_Register_t*) PORT_REGISTER_BASE_ADDR)
#define PCIMSK_REGISTER_BASE_PTR ((volatile uint8_t*) PCIMSK_REGISTER_BASE_ADDR)

#endif