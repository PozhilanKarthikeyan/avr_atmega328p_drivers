#include "timer/timer.h"
#include "gpio/gpio.h"
#include "stdint.h"

volatile uint32_t millis_tick=0;

void timer_callback(void){
    millis_tick++;
}


drv_timer_config_t timer_config={
    .device=DRV_TIMER_0,
    .clk_src=DRV_TIMER_CLK_IO_DIV_64,
    .timer_mode=DRV_TIMER_MODE_CTC,
    .overflow_interrupt_enable=true,
    .overflow_callback=timer_callback,
    .OCA_config={
        .output_cmp_value=100, // total count is 250 , i.e 0-249
        .output_cmp_mode=DRV_TIMER_OUTPUT_CMP_PIN_DISCONNECTED,
        .interrupt_enable=false,
    },
    .OCB_config={
        .interrupt_enable=false,
    },
};

uint32_t millis(){
    return millis_tick;
}

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

int main(){
    drv_timer_init(&timer_config);
    drv_gpio_init(&led_config);
    drv_timer_start(&timer_config);
    uint32_t previous_millis_tick=0;
    while (1)
    {
        if ((millis()-previous_millis_tick)>=1000U) //one second
        {
            drv_gpio_toggle_output(&led_config);
        }
        previous_millis_tick=millis();
        
    }
    
}
