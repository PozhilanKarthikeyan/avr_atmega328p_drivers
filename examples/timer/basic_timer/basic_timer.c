#include "timer/timer.h"
#include "avr/interrupt.h"
#include <gpio/gpio.h>
#include <avr/io.h>

volatile uint16_t accumulator=0;

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

void timer_callback(void){
    //approximately 1 Second delay
    if (accumulator>=1000)
    {
        drv_gpio_toggle_output(&led_config);
        accumulator=0;
    }
    else accumulator++;
    
}

drv_timer_config_t timer_config={
    .device=DRV_TIMER_0,
    .clk_src=DRV_TIMER_CLK_IO_DIV_64,
    .timer_mode=DRV_TIMER_MODE_NORMAL,
    .overflow_interrupt_enable=true,
    .overflow_callback=timer_callback,
    .OCA_config={
        .interrupt_enable=false,
    },
    .OCB_config={
        .interrupt_enable=false,
    },
};

int main(){
    drv_gpio_init(&led_config);
    drv_timer_init(&timer_config);
    sei();
    drv_timer_start(&timer_config);
    while (1){
    }
    
}

