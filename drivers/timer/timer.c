#include "timer.h"
#include "common/helper_macros.h"
#include <avr/io.h>
#include <string.h>
#include <avr/interrupt.h>

drv_timer_callback_t timer0_OVF_callback=NULL;
drv_timer_callback_t timer0_COMPB_callback=NULL;
drv_timer_callback_t timer0_COMPA_callback=NULL;
drv_timer_callback_t timer1_OVF_callback=NULL;
drv_timer_callback_t timer1_COMPB_callback=NULL;
drv_timer_callback_t timer1_COMPA_callback=NULL;
drv_timer_callback_t timer1_CAPT_callback=NULL;
drv_timer_callback_t timer2_OVF_callback=NULL;
drv_timer_callback_t timer2_COMPB_callback=NULL;
drv_timer_callback_t timer2_COMPA_callback=NULL;

static inline void isr_helper(drv_timer_callback_t callback){
    if (callback!=NULL)
    {
        callback();
    } 
}

ISR(TIMER0_OVF_vect){
    isr_helper(timer0_OVF_callback);
}

ISR(TIMER0_COMPB_vect){
    isr_helper(timer0_COMPB_callback);
}

ISR(TIMER0_COMPA_vect){
    isr_helper(timer0_COMPA_callback);    
}

ISR(TIMER1_OVF_vect){
    isr_helper(timer1_OVF_callback); 
}

ISR(TIMER1_COMPB_vect){
    isr_helper(timer1_COMPB_callback);    
}

ISR(TIMER1_COMPA_vect){
    isr_helper(timer1_COMPA_callback);    
}

ISR(TIMER1_CAPT_vect){
    isr_helper(timer1_CAPT_callback);   
}

ISR(TIMER2_OVF_vect){
    isr_helper(timer2_OVF_callback);  
}

ISR(TIMER2_COMPB_vect){
    isr_helper(timer2_COMPB_callback);    
}

ISR(TIMER2_COMPA_vect){
    isr_helper(timer2_COMPA_callback);    
}

drv_timer_error_t drv_timer_init(drv_timer_config_t* timer_config){
    // TODO: Validate COM modes for PWM modes where OCRA is used as TOP (WGM = 5 or 7).
    //In these modes, OCA has hardware restrictions and some COMA settings are reserved.
    switch (timer_config->device)
    {
    case DRV_TIMER_0:
        WRITE_FIELD(TIMER0->TCCRA,COM0A0,2,timer_config->OCA_config.output_cmp_mode); 
        WRITE_FIELD(TIMER0->TCCRA,COM0B0,2,timer_config->OCB_config.output_cmp_mode); 
        TIMER0->OCRA=timer_config->OCA_config.output_cmp_value;
        TIMER0->OCRB=timer_config->OCB_config.output_cmp_value;
        switch (timer_config->timer_mode)
        {
        case DRV_TIMER_MODE_NORMAL:
            WRITE_FIELD(TIMER0->TCCRA,WGM00,2,0b00);
            CLEAR_BIT(TIMER0->TCCRB,WGM02);
            break;
        case DRV_TIMER_MODE_CTC:
            WRITE_FIELD(TIMER0->TCCRA,WGM00,2,0b10);
            CLEAR_BIT(TIMER0->TCCRB,WGM02);
            break;

        case DRV_TIMER_MODE_FAST_PWM_TOP_MAX:
            WRITE_FIELD(TIMER0->TCCRA,WGM00,2,0b11);
            CLEAR_BIT(TIMER0->TCCRB,WGM02);
            break;

        case DRV_TIMER_MODE_FAST_PWM_TOP_OCRA:
            WRITE_FIELD(TIMER0->TCCRA,WGM00,2,0b11);
            SET_BIT(TIMER0->TCCRB,WGM02);
            break;

        case DRV_TIMER_MODE_PWM_PHASE_CORRECT_TOP_MAX:
            WRITE_FIELD(TIMER0->TCCRA,WGM00,2,0b01);
            CLEAR_BIT(TIMER0->TCCRB,WGM02);
            break;

        case DRV_TIMER_MODE_PWM_PHASE_CORRECT_TOP_OCRA:
            WRITE_FIELD(TIMER0->TCCRA,WGM00,2,0b01);
            SET_BIT(TIMER0->TCCRB,WGM02);
            break;
        
        default:
            return DRV_TIMER_ERROR_INVALID_MODE;
        }
        if (timer_config->overflow_interrupt_enable){
            SET_BIT(TIMSK0,TOIE0);
            timer0_OVF_callback=timer_config->overflow_callback;
        } 
        else CLEAR_BIT(TIMSK0,TOIE0);
        if (timer_config->OCA_config.interrupt_enable){
            SET_BIT(TIMSK0,OCIE0A);
            timer0_COMPA_callback=timer_config->OCA_config.callback;
        } 
        else CLEAR_BIT(TIMSK0,OCIE0A);
        if (timer_config->OCB_config.interrupt_enable){
            SET_BIT(TIMSK0,OCIE0B);
            timer0_COMPB_callback=timer_config->OCB_config.callback;
        } 
        else CLEAR_BIT(TIMSK0,OCIE0B);
        break;

    case DRV_TIMER_1:
        //TODO
        break;

    case DRV_TIMER_2:
        //TODO: async mode
        WRITE_FIELD(TIMER2->TCCRA,COM2A0,2,timer_config->OCA_config.output_cmp_mode); 
        WRITE_FIELD(TIMER2->TCCRA,COM2B0,2,timer_config->OCB_config.output_cmp_mode); 
        TIMER2->OCRA=timer_config->OCA_config.output_cmp_value;
        TIMER2->OCRB=timer_config->OCB_config.output_cmp_value;
        switch (timer_config->timer_mode)
        {
        case DRV_TIMER_MODE_NORMAL:
            WRITE_FIELD(TIMER2->TCCRA,WGM20,2,0b00);
            CLEAR_BIT(TIMER2->TCCRB,WGM22);
            break;
        case DRV_TIMER_MODE_CTC:
            WRITE_FIELD(TIMER2->TCCRA,WGM20,2,0b10);
            CLEAR_BIT(TIMER2->TCCRB,WGM22);
            break;

        case DRV_TIMER_MODE_FAST_PWM_TOP_MAX:
            WRITE_FIELD(TIMER2->TCCRA,WGM20,2,0b11);
            CLEAR_BIT(TIMER2->TCCRB,WGM22);
            break;

        case DRV_TIMER_MODE_FAST_PWM_TOP_OCRA:
            WRITE_FIELD(TIMER2->TCCRA,WGM20,2,0b11);
            SET_BIT(TIMER2->TCCRB,WGM22);
            break;

        case DRV_TIMER_MODE_PWM_PHASE_CORRECT_TOP_MAX:
            WRITE_FIELD(TIMER2->TCCRA,WGM20,2,0b01);
            CLEAR_BIT(TIMER2->TCCRB,WGM22);
            break;

        case DRV_TIMER_MODE_PWM_PHASE_CORRECT_TOP_OCRA:
            WRITE_FIELD(TIMER2->TCCRA,WGM20,2,0b01);
            SET_BIT(TIMER2->TCCRB,WGM22);
            break;
        
        default:
            return DRV_TIMER_ERROR_INVALID_MODE;
        }

        if (timer_config->overflow_interrupt_enable){
            SET_BIT(TIMSK2,TOIE2);
            timer2_OVF_callback=timer_config->overflow_callback;
        } 
        else CLEAR_BIT(TIMSK2,TOIE2);
        if (timer_config->OCA_config.interrupt_enable){
            SET_BIT(TIMSK2,OCIE2A);
            timer2_COMPA_callback=timer_config->OCA_config.callback;
        } 
        else CLEAR_BIT(TIMSK2,OCIE2A);
        if (timer_config->OCB_config.interrupt_enable){
            SET_BIT(TIMSK2,OCIE2B);
            timer2_COMPB_callback=timer_config->OCB_config.callback;
        } 
        else CLEAR_BIT(TIMSK2,OCIE2B);
        break;
    
    default:
        return DRV_TIMER_ERROR_INVALID_DEVICE;
    }
    return DRV_TIMER_SUCCESS;
}

drv_timer_error_t drv_timer_deinit(drv_timer_config_t* timer_config){
    drv_timer_stop(timer_config);
    switch (timer_config->device)
    {
    case DRV_TIMER_0:
        memset(TIMER0,0,sizeof(*TIMER0));
        timer0_OVF_callback=NULL;
        timer0_COMPB_callback=NULL;
        timer0_COMPA_callback=NULL;
        break;
    case DRV_TIMER_1:
        memset(TIMER1,0,sizeof(*TIMER1));
        timer1_OVF_callback=NULL;
        timer1_COMPB_callback=NULL;
        timer1_COMPA_callback=NULL;
        timer1_CAPT_callback=NULL;
        break;
    case DRV_TIMER_2:
        memset(TIMER2,0,sizeof(*TIMER2));
        timer2_OVF_callback=NULL;
        timer2_COMPB_callback=NULL;
        timer2_COMPA_callback=NULL;
        break;
    default:
        return DRV_TIMER_ERROR_INVALID_DEVICE;
    }
    return DRV_TIMER_SUCCESS;

}

drv_timer_error_t drv_timer_start(drv_timer_config_t* timer_config){
    if (timer_config->clk_src>=DRV_TIMER_CLK_SRC_MAX)
    {
        return DRV_TIMER_ERROR_INVALID_CLK_SOURCE;
    }
    switch (timer_config->device)
    {
    case DRV_TIMER_0:
        WRITE_FIELD(TIMER0->TCCRB,CS00,3,timer_config->clk_src);
        break;
    case DRV_TIMER_1:
        WRITE_FIELD(TIMER1->TCCRB,CS10,3,timer_config->clk_src);
        break;
    case DRV_TIMER_2:
        WRITE_FIELD(TIMER2->TCCRB,CS20,3,timer_config->clk_src);
        break;
    default:
        return DRV_TIMER_ERROR_INVALID_DEVICE;
    }
    return DRV_TIMER_SUCCESS;
}
drv_timer_error_t drv_timer_stop(drv_timer_config_t* timer_config){
    switch (timer_config->device)
    {
    case DRV_TIMER_0:
        WRITE_FIELD(TIMER0->TCCRB,CS00,3,0);
        break;
    case DRV_TIMER_1:
        WRITE_FIELD(TIMER1->TCCRB,CS10,3,0);
        break;
    case DRV_TIMER_2:
        WRITE_FIELD(TIMER2->TCCRB,CS20,3,0);
        break;
    default:
        return DRV_TIMER_ERROR_INVALID_DEVICE;
    }
    return DRV_TIMER_SUCCESS;

}
drv_timer_error_t drv_get_counter_value(drv_timer_config_t* timer_config,uint16_t* value){
    switch (timer_config->device)
    {
    case DRV_TIMER_0:
        *value=TIMER0->TCNT;
        break;

    case DRV_TIMER_1:
        *value=TIMER1->TCNT;
        break;

    case DRV_TIMER_2:
        *value=TIMER2->TCNT;
        break;
    
    default:
        return DRV_TIMER_ERROR_INVALID_DEVICE;
    }
    return DRV_TIMER_SUCCESS;

}

//update the config before calling the function
drv_timer_error_t drv_update_output_cmp_value(drv_timer_config_t* timer_config,drv_output_cmp_regs_t reg){
    switch (timer_config->device)
    {
    case DRV_TIMER_0:
        if (reg==DRV_OUTPUT_CMP_REG_A)
        {
            TIMER0->OCRA=timer_config->OCA_config.output_cmp_value;
        }
        else if (reg==DRV_OUTPUT_CMP_REG_B)
        {
            TIMER0->OCRB=timer_config->OCB_config.output_cmp_value;
        }
        else
        {
            return DRV_TIMER_ERROR_INVALID_OUTPUT_CMP_REG;
        }
        break;

    case DRV_TIMER_1:
        //TODO
        break;

    case DRV_TIMER_2:
        if (reg==DRV_OUTPUT_CMP_REG_A)
        {
            TIMER2->OCRA=timer_config->OCA_config.output_cmp_value;
        }
        else if (reg==DRV_OUTPUT_CMP_REG_B)
        {
            TIMER2->OCRB=timer_config->OCB_config.output_cmp_value;
        }
        else
        {
            return DRV_TIMER_ERROR_INVALID_OUTPUT_CMP_REG;
        }
        break;
    
    default:
        return DRV_TIMER_ERROR_INVALID_DEVICE;
    }
    return DRV_TIMER_SUCCESS;
}

drv_timer_error_t drv_force_output_cmp(drv_timer_config_t* timer_config,drv_output_cmp_regs_t reg){
    if (timer_config->timer_mode!=DRV_TIMER_MODE_NORMAL&&timer_config->timer_mode!=DRV_TIMER_MODE_CTC)
    {
        return DRV_TIMER_ERROR_INVALID_MODE;
    }
    
    switch (timer_config->device)
    {
    case DRV_TIMER_0:
        if (reg==DRV_OUTPUT_CMP_REG_A)
        {
            SET_BIT(TIMER0->TCCRB,FOC0A);
        }
        else if (reg==DRV_OUTPUT_CMP_REG_B)
        {
            SET_BIT(TIMER0->TCCRB,FOC0B);
        }
        else
        {
            return DRV_TIMER_ERROR_INVALID_OUTPUT_CMP_REG;
        }
        break;

    case DRV_TIMER_1:
        //TODO
        break;

    case DRV_TIMER_2:
        if (reg==DRV_OUTPUT_CMP_REG_A)
        {
            SET_BIT(TIMER2->TCCRB,FOC2A);
        }
        else if (reg==DRV_OUTPUT_CMP_REG_B)
        {
            SET_BIT(TIMER2->TCCRB,FOC2B);
        }
        else
        {
            return DRV_TIMER_ERROR_INVALID_OUTPUT_CMP_REG;
        }
        break;
    
    default:
        return DRV_TIMER_ERROR_INVALID_DEVICE;
    }
    return DRV_TIMER_SUCCESS;
}