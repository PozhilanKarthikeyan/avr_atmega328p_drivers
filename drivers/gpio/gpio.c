#include "gpio.h"

static int const MAX_PORT_PIN_NUMBER[]={MAX_PORTB_PIN_NUMBER,MAX_PORTC_PIN_NUMBER,MAX_PORTD_PIN_NUMBER};

static volatile uint8_t previous_gpio_state[DRV_GPIO_PORT_MAX]={0}; 

static volatile drv_gpio_interrupt_mode_t gpio_interrupt_mode_array[DRV_GPIO_PORT_MAX][MAX_PORTB_PIN_NUMBER+1]={0};
static drv_gpio_callback_t global_gpio_callback=NULL;

static inline void _isr_helper(drv_port_t port){
    if (global_gpio_callback == NULL) {
        return ;
    }
    uint8_t pin_reg = PORT_REGISTER_BASE_PTR[port].PIN;
    uint8_t toggled_pins = previous_gpio_state[port]^pin_reg;
    uint8_t change_pin = 0;
    
    for (size_t i = 0; i < sizeof(toggled_pins) * 8; i++)  //converting bytes to bits
    {
        if (READ_BIT(toggled_pins,i))
        {
            change_pin = i;
        }
        else
        {
            continue;
        }
        
        switch (gpio_interrupt_mode_array[port][change_pin])
        {
        case DRV_GPIO_INTERRUPT_MODE_BOTH_EDGES:
            global_gpio_callback(port,change_pin,READ_BIT(pin_reg,change_pin));
            break;
    
        case DRV_GPIO_INTERRUPT_MODE_RISING_EDGE:
            if (READ_BIT(pin_reg, change_pin))
            {
                global_gpio_callback(port,change_pin,DRV_GPIO_STATE_HIGH);
            }
            break;
        case DRV_GPIO_INTERRUPT_MODE_FALLING_EDGE:
            if (!READ_BIT(pin_reg, change_pin)) {
                global_gpio_callback(port, change_pin, DRV_GPIO_STATE_LOW);
            }
            break;
    
        default:
            break;
        }
    }
    
    previous_gpio_state[port] = pin_reg;
}

ISR(PCINT0_vect){
    _isr_helper(DRV_GPIO_PORT_B);
}

ISR_ALIAS(PCINT1_vect, PCINT0_vect);
ISR_ALIAS(PCINT2_vect, PCINT0_vect);

drv_gpio_error_t drv_gpio_init(drv_gpio_config_t* gpio_config){
    if (gpio_config->port >= DRV_GPIO_PORT_MAX)
    {
        return DRV_GPIO_ERROR_INVALID_PORT;
    }
    if (gpio_config->pin > MAX_PORT_PIN_NUMBER[gpio_config->port]){
        return DRV_GPIO_ERROR_INVALID_PIN;
    }
    
    if (gpio_config->interrupt_config.enable == true)
    {
        SET_BIT(PCIMSK_REGISTER_BASE_PTR[gpio_config->port], gpio_config->pin);
        SET_BIT(PCICR, gpio_config->port);
        gpio_interrupt_mode_array[gpio_config->port][gpio_config->pin] = gpio_config->interrupt_config.drv_gpio_interrupt_mode;
    }
    
    switch (gpio_config->mode)
    {
        case DRV_GPIO_MODE_INPUT_HIGH_Z:
            CLEAR_BIT(PORT_REGISTER_BASE_PTR[gpio_config->port].DDR, gpio_config->pin);
            CLEAR_BIT(PORT_REGISTER_BASE_PTR[gpio_config->port].PORT, gpio_config->pin);
            break;
        case DRV_GPIO_MODE_INPUT_PULLUP:
            CLEAR_BIT(PORT_REGISTER_BASE_PTR[gpio_config->port].DDR, gpio_config->pin);
            SET_BIT(PORT_REGISTER_BASE_PTR[gpio_config->port].PORT, gpio_config->pin);
            break;
        case DRV_GPIO_MODE_OUTPUT:
            if (gpio_config->inital_state == DRV_GPIO_STATE_HIGH)
            {
                SET_BIT(PORT_REGISTER_BASE_PTR[gpio_config->port].PORT, gpio_config->pin);
            }
            else if (gpio_config->inital_state == DRV_GPIO_STATE_LOW)
            {
                CLEAR_BIT(PORT_REGISTER_BASE_PTR[gpio_config->port].PORT, gpio_config->pin); 
            }
            else{
                return DRV_GPIO_ERROR_INVALID_STATE;
            }
            SET_BIT(PORT_REGISTER_BASE_PTR[gpio_config->port].DDR, gpio_config->pin);
            break;    
    default:
        return DRV_GPIO_ERROR_INVALID_MODE;
    }
    
    previous_gpio_state[gpio_config->port] = PORT_REGISTER_BASE_PTR[gpio_config->port].PIN;
    return DRV_GPIO_SUCCESS;
}

drv_gpio_error_t drv_gpio_deinit(drv_gpio_config_t* gpio_config){
    gpio_config->mode = DRV_GPIO_MODE_INPUT_PULLUP;
    drv_gpio_error_t error = drv_gpio_init(gpio_config);
    return error;
}

drv_gpio_error_t drv_gpio_set_output(drv_gpio_config_t* gpio_config){
    if (gpio_config->mode != DRV_GPIO_MODE_OUTPUT){
        return DRV_GPIO_ERROR_INVALID_MODE;
    }
    SET_BIT(PORT_REGISTER_BASE_PTR[gpio_config->port].PORT, gpio_config->pin);
    return DRV_GPIO_SUCCESS;
}
drv_gpio_error_t drv_gpio_clear_output(drv_gpio_config_t* gpio_config){
    if (gpio_config->mode != DRV_GPIO_MODE_OUTPUT){
        return DRV_GPIO_ERROR_INVALID_MODE;
    }
    CLEAR_BIT(PORT_REGISTER_BASE_PTR[gpio_config->port].PORT, gpio_config->pin);
    return DRV_GPIO_SUCCESS;
}
drv_gpio_error_t drv_gpio_toggle_output(drv_gpio_config_t* gpio_config){
    if (gpio_config->mode!=DRV_GPIO_MODE_OUTPUT){
        return DRV_GPIO_ERROR_INVALID_MODE;
    }
    SET_BIT(PORT_REGISTER_BASE_PTR[gpio_config->port].PIN,gpio_config->pin);
    return DRV_GPIO_SUCCESS;
}

drv_gpio_error_t drv_gpio_get_state(drv_gpio_config_t* gpio_config, drv_gpio_state_t* state){
    *state = READ_BIT(PORT_REGISTER_BASE_PTR[gpio_config->port].PIN, gpio_config->pin);
    return DRV_GPIO_SUCCESS;
}

drv_gpio_error_t drv_gpio_register_interrupt_callback(drv_gpio_callback_t callback){
    global_gpio_callback = callback;
    return DRV_GPIO_SUCCESS;
}