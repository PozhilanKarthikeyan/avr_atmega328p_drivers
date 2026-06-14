#ifndef GPIO_H
#define GPIO_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include "common/helper_macros.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_PORTB_PIN_NUMBER 7
#define MAX_PORTC_PIN_NUMBER 6
#define MAX_PORTD_PIN_NUMBER 7

typedef enum drv_port_e{
    DRV_GPIO_PORT_B,
    DRV_GPIO_PORT_C,
    DRV_GPIO_PORT_D,
    DRV_GPIO_PORT_MAX,
}drv_port_t;

typedef enum drv_mode_e{
    DRV_GPIO_MODE_INPUT_HIGH_Z,
    DRV_GPIO_MODE_INPUT_PULLUP,
    DRV_GPIO_MODE_OUTPUT,
    DRV_GPIO_MODE_MAX,
}drv_mode_t;

typedef enum drv_gpio_state_e{
    DRV_GPIO_STATE_LOW,
    DRV_GPIO_STATE_HIGH,
    DRV_GPIO_STATE_MAX,
}drv_gpio_state_t;

typedef enum drv_gpio_error_e{
    DRV_GPIO_SUCCESS,
    DRV_GPIO_ERROR_INVALID_PORT,
    DRV_GPIO_ERROR_INVALID_PIN,
    DRV_GPIO_ERROR_INVALID_MODE,
    DRV_GPIO_ERROR_INVALID_STATE,
}drv_gpio_error_t;

typedef enum drv_gpio_interrupt_mode_e{
    DRV_GPIO_INTERRUPT_MODE_NONE,
    DRV_GPIO_INTERRUPT_MODE_RISING_EDGE,
    DRV_GPIO_INTERRUPT_MODE_FALLING_EDGE,
    DRV_GPIO_INTERRUPT_MODE_BOTH_EDGES,
    DRV_GPIO_INTERRUPT_MODE_MAX,
}drv_gpio_interrupt_mode_t;

typedef void (*drv_gpio_callback_t)(drv_port_t,uint8_t,drv_gpio_state_t);

typedef struct drv_gpio_interrupt_config_s
{
    bool enable;
    drv_gpio_interrupt_mode_t drv_gpio_interrupt_mode;
}drv_gpio_interrupt_config_t;


typedef struct drv_gpio_config_s{
    drv_port_t port;   
    uint8_t pin;                   //eg. for PB5 port = DRV_GPIO_PORT_B , pin = 5
    drv_mode_t mode;
    drv_gpio_state_t inital_state; //initial state of the pin, only valid if mode is output
    drv_gpio_interrupt_config_t interrupt_config;
}drv_gpio_config_t;

drv_gpio_error_t drv_gpio_init(drv_gpio_config_t* gpio_config);
drv_gpio_error_t drv_gpio_register_interrupt_callback(drv_gpio_callback_t callback);
drv_gpio_error_t drv_gpio_deinit(drv_gpio_config_t* gpio_config);
drv_gpio_error_t drv_gpio_set_output(drv_gpio_config_t* gpio_config);
drv_gpio_error_t drv_gpio_clear_output(drv_gpio_config_t* gpio_config);
drv_gpio_error_t drv_gpio_toggle_output(drv_gpio_config_t* gpio_config);
drv_gpio_error_t drv_gpio_get_state(drv_gpio_config_t* gpio_config,drv_gpio_state_t* state);


#endif