#include <gpio/gpio.h>

drv_gpio_config_t led_config={
    .port=DRV_GPIO_PORT_B,
    .pin=5,
    .mode=DRV_GPIO_MODE_OUTPUT
};

drv_gpio_config_t btn_config={
    .port=DRV_GPIO_PORT_B,
    .pin=0,
    .mode=DRV_GPIO_MODE_INPUT_PULLUP
};

int main(){
    drv_gpio_init(&led_config);
    drv_gpio_init(&btn_config);
    drv_gpio_state_t btn_state;
    while(1){
        drv_gpio_get_state(&btn_config,&btn_state);
        if (btn_state==DRV_GPIO_STATE_LOW){
            drv_gpio_set_output(&led_config);
        }
        else{
            drv_gpio_clear_output(&led_config);
        }
    }
}