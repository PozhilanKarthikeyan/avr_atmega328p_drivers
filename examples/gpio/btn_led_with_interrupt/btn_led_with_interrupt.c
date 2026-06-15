#include <gpio/gpio.h>
#include <util/delay.h>
#include <avr/interrupt.h>

drv_gpio_config_t led_config={
    .port=DRV_GPIO_PORT_B,
    .pin=5,
    .mode=DRV_GPIO_MODE_OUTPUT,
    .inital_state=DRV_GPIO_STATE_LOW,
    .interrupt_config={
        .enable=false,
        .drv_gpio_interrupt_mode=DRV_GPIO_INTERRUPT_MODE_NONE,
    }
};

drv_gpio_config_t btn_config={
    .port=DRV_GPIO_PORT_B,
    .pin=0,
    .mode=DRV_GPIO_MODE_INPUT_PULLUP,
    .inital_state=DRV_GPIO_STATE_HIGH, //Ignored if the mode is not output
    .interrupt_config={
        .enable=true,
        .drv_gpio_interrupt_mode=DRV_GPIO_INTERRUPT_MODE_BOTH_EDGES,
    }
};

void gpio_callback(drv_port_t port,uint8_t pin,drv_gpio_state_t state){
    if (state==DRV_GPIO_STATE_LOW){
        drv_gpio_set_output(&led_config);
    }
    else{
        drv_gpio_clear_output(&led_config);
    }

}


int main(){
    drv_gpio_register_interrupt_callback(gpio_callback);
    sei(); //setting the global interrupt mask
    drv_gpio_init(&led_config);
    drv_gpio_init(&btn_config);
    while(1){//do nothing
        _delay_ms(10);
    } 
}