#ifndef TIMER_H
#define TIMER_H

#include "timer_internal.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum drv_timer_device_e{
    DRV_TIMER_0,
    DRV_TIMER_1,
    DRV_TIMER_2,
    DRV_TIMER_COUNT,
}drv_timer_device_t;

typedef enum drv_timer_clock_src_e{
    DRV_TIMER_CLK_DISCONNECTED      =0,

    //USE FOR TIMER 0 AND 1
    DRV_TIMER_CLK_IO_DIV_1          =1,
    DRV_TIMER_CLK_IO_DIV_8          =2,
    DRV_TIMER_CLK_IO_DIV_64         =3,
    DRV_TIMER_CLK_IO_DIV_256        =4,
    DRV_TIMER_CLK_IO_DIV_1024       =5,
    DRV_TIMER_CLK_EXT_FALLING       =6,
    DRV_TIMER_CLK_EXT_RISING        =7,

    //USE FOR TIMER 2 
    DRV_TIMER_CLK_T2S_DIV_1         =1,
    DRV_TIMER_CLK_T2S_DIV_8         =2,
    DRV_TIMER_CLK_T2S_DIV_32        =3,
    DRV_TIMER_CLK_T2S_DIV_64        =4,
    DRV_TIMER_CLK_T2S_DIV_128       =5,
    DRV_TIMER_CLK_T2S_DIV_256       =6,
    DRV_TIMER_CLK_T2S_DIV_1024      =7,

    DRV_TIMER_CLK_SRC_MAX           =8,

}drv_timer_clock_src_t;

typedef enum drv_timer_mode_e{
    //NON PWM MODES
    DRV_TIMER_MODE_NORMAL,
    DRV_TIMER_MODE_CTC,

    //PWM MODES
    DRV_TIMER_MODE_FAST_PWM_TOP_MAX,
    DRV_TIMER_MODE_FAST_PWM_TOP_OCRA,
    DRV_TIMER_MODE_PWM_PHASE_CORRECT_TOP_MAX,
    DRV_TIMER_MODE_PWM_PHASE_CORRECT_TOP_OCRA,
}drv_timer_mode_t;

typedef enum drv_timer_output_cmp_modes_e{
    DRV_TIMER_OUTPUT_CMP_PIN_DISCONNECTED    =0,
    DRV_TIMER_OUTPUT_CMP_PIN_TOGGLE          =1,

    //IN NON PWM MODES
    DRV_TIMER_OUTPUT_CMP_PIN_CLEAR           =2,
    DRV_TIMER_OUTPUT_CMP_PIN_SET             =3,

    //IN PWM MODES
    DRV_TIMER_OUTPUT_CMP_PWM_NON_INVERTING   =2, 
    DRV_TIMER_OUTPUT_CMP_PWM_INVERTING       =3,

    DRV_TIMER_OUTPUT_CMP_MODE_MAX =4,

}drv_timer_output_cmp_modes_t;

typedef void (*drv_timer_callback_t)(void);


typedef struct drv_timer_output_cmp_config_s
{
    drv_timer_output_cmp_modes_t output_cmp_mode;
    bool interrupt_enable;
    drv_timer_callback_t callback;
    uint16_t output_cmp_value;

}drv_timer_output_cmp_config_t;

typedef enum drv_timer_error_e{
    DRV_TIMER_SUCCESS,
    DRV_TIMER_ERROR_INVALID_DEVICE,
    DRV_TIMER_ERROR_INVALID_MODE,
    DRV_TIMER_ERROR_INVALID_CLK_SOURCE,
    DRV_TIMER_ERROR_INVALID_OUTPUT_CMP_MODE,
}drv_timer_error_t;



typedef struct drv_timer_config_s
{
   drv_timer_device_t device;
   drv_timer_clock_src_t clk_src;
   drv_timer_mode_t timer_mode;
   bool overflow_interrupt_enable;
   drv_timer_callback_t overflow_callback;
   drv_timer_output_cmp_config_t OCA_config;
   drv_timer_output_cmp_config_t OCB_config;

}drv_timer_config_t;

drv_timer_error_t drv_timer_init(drv_timer_config_t* timer_config);
drv_timer_error_t drv_timer_deinit(drv_timer_config_t* timer_config);
drv_timer_error_t drv_timer_start(drv_timer_config_t* timer_config);
drv_timer_error_t drv_timer_stop(drv_timer_config_t* timer_config);
drv_timer_error_t drv_get_counter_value(drv_timer_config_t* timer_config,uint16_t* value);

#endif