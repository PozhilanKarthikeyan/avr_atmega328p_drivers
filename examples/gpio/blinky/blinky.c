#include <gpio/gpio.h>
#include <util/delay.h>

drv_gpio_config_t led_config={
    .port=DRV_GPIO_PORT_B,
    .pin=5,
    .mode=DRV_GPIO_MODE_OUTPUT
};

int main(){
    drv_gpio_init(&led_config);
    while(1){
        drv_gpio_toggle_output(&led_config);
        _delay_ms(1000);
    }
}

