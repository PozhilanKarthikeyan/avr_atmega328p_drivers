#include "gpio.h"

static volatile uint8_t* const DDR_ARR[]={&DDRB,&DDRC,&DDRD};
static volatile uint8_t* const PORT_ARR[]={&PORTB,&PORTC,&PORTD};
static volatile uint8_t* const PIN_ARR[]={&PINB,&PINC,&PIND};
static int const MAX_PORT_PIN_NUMBER[]={MAX_PORTB_PIN_NUMBER,MAX_PORTC_PIN_NUMBER,MAX_PORTD_PIN_NUMBER};


drv_gpio_error_t drv_gpio_init(drv_gpio_config_t* gpio_config){
    if (gpio_config->port>=DRV_GPIO_PORT_MAX)
    {
        return DRV_GPIO_ERROR_INVALID_PORT;
    }
    if (gpio_config->pin>MAX_PORT_PIN_NUMBER[gpio_config->port]){
        return DRV_GPIO_ERROR_INVALID_PIN;
    }
    switch (gpio_config->mode)
    {
        case DRV_GPIO_MODE_INPUT_HIGH_Z:
            CLEAR_BIT(*(DDR_ARR[gpio_config->port]),gpio_config->pin);
            CLEAR_BIT(*(PORT_ARR[gpio_config->port]),gpio_config->pin);
            break;
        case DRV_GPIO_MODE_INPUT_PULLUP:
            CLEAR_BIT(*(DDR_ARR[gpio_config->port]),gpio_config->pin);
            SET_BIT(*(PORT_ARR[gpio_config->port]),gpio_config->pin);
            break;
        case DRV_GPIO_MODE_OUTPUT:
            SET_BIT(*(DDR_ARR[gpio_config->port]),gpio_config->pin);
            break;    
    default:
        return DRV_GPIO_ERROR_INVALID_MODE;
    }
    return DRV_GPIO_SUCCESS;
}

drv_gpio_error_t drv_gpio_deinit(drv_gpio_config_t* gpio_config){
    gpio_config->mode=DRV_GPIO_MODE_INPUT_HIGH_Z;
    drv_gpio_error_t error=drv_gpio_init(gpio_config);
    return error;
}

drv_gpio_error_t drv_gpio_set_output(drv_gpio_config_t* gpio_config){
    if (gpio_config->mode!=DRV_GPIO_MODE_OUTPUT){
        return DRV_GPIO_ERROR_INVALID_MODE;
    }
    SET_BIT(*(PORT_ARR[gpio_config->port]),gpio_config->pin);
    return DRV_GPIO_SUCCESS;
}
drv_gpio_error_t drv_gpio_clear_output(drv_gpio_config_t* gpio_config){
    if (gpio_config->mode!=DRV_GPIO_MODE_OUTPUT){
        return DRV_GPIO_ERROR_INVALID_MODE;
    }
    CLEAR_BIT(*(PORT_ARR[gpio_config->port]),gpio_config->pin);
    return DRV_GPIO_SUCCESS;
}
drv_gpio_error_t drv_gpio_toggle_output(drv_gpio_config_t* gpio_config){
    if (gpio_config->mode!=DRV_GPIO_MODE_OUTPUT){
        return DRV_GPIO_ERROR_INVALID_MODE;
    }
    SET_BIT(*(PIN_ARR[gpio_config->port]),gpio_config->pin);
    return DRV_GPIO_SUCCESS;
}
drv_gpio_error_t drv_gpio_get_state(drv_gpio_config_t* gpio_config,drv_gpio_state_t* state){
    if (gpio_config->mode==DRV_GPIO_MODE_OUTPUT)
    {
        *state = READ_BIT(*(PORT_ARR[gpio_config->port]),gpio_config->pin);
    }
    else if (gpio_config->mode==DRV_GPIO_MODE_INPUT_PULLUP || gpio_config->mode==DRV_GPIO_MODE_INPUT_HIGH_Z)
    {
        *state = READ_BIT(*(PIN_ARR[gpio_config->port]),gpio_config->pin);
    }
    else{
        return DRV_GPIO_ERROR_INVALID_MODE;
    }
    return DRV_GPIO_SUCCESS;
}


