#include "timer.h"
#include "common/helper_macros.h"
#include <avr/io.h>
#include <avr/cpufunc.h>
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

static inline bool drv_timer1_oc1a_toggle_supported(const drv_timer_config_t *timer_config)
{
    switch (timer_config->timer_mode)
    {
        case DRV_TIMER_MODE_NORMAL:
            return true;
        case DRV_TIMER_MODE_CTC:
            return true;

        case DRV_TIMER_MODE_FAST_PWM:
            return (timer_config->top == DRV_TIMER_TOP_ICR) ||
                   (timer_config->top == DRV_TIMER_TOP_OCRA);

        case DRV_TIMER_MODE_PWM_PHASE_CORRECT:
            return (timer_config->top == DRV_TIMER_TOP_OCRA);
        case DRV_TIMER_MODE_PWM_PHASE_FREQ_CORRECT:
            return (timer_config->top == DRV_TIMER_TOP_OCRA);

        default:
            return false;
    }
}

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
    if (timer_config->OCB_config.output_cmp_mode==DRV_TIMER_OUTPUT_CMP_PIN_TOGGLE &&(timer_config->timer_mode!=DRV_TIMER_MODE_NORMAL && timer_config->timer_mode!=DRV_TIMER_MODE_CTC))
    {
        return DRV_TIMER_ERROR_INVALID_OUTPUT_CMP_MODE;
    }
    switch (timer_config->device)
    {
    case DRV_TIMER_0:
        switch (timer_config->timer_mode)
        {
        case DRV_TIMER_MODE_NORMAL:
            CLEAR_BIT(TIMER0->TCCRB,WGM02);
            WRITE_FIELD(TIMER0->TCCRA,WGM00,2,0b00);
            break;
        case DRV_TIMER_MODE_CTC:
            if (timer_config->top!=DRV_TIMER_TOP_OCRA)
            {
                return DRV_TIMER_ERROR_INVALID_TOP;
            }
            CLEAR_BIT(TIMER0->TCCRB,WGM02);
            WRITE_FIELD(TIMER0->TCCRA,WGM00,2,0b10);
            break;

        case DRV_TIMER_MODE_FAST_PWM:
            if (timer_config->top==DRV_TIMER_TOP_MAX)
            {
                CLEAR_BIT(TIMER0->TCCRB,WGM02);
            }
            else if(timer_config->top==DRV_TIMER_TOP_OCRA)
            {
                SET_BIT(TIMER0->TCCRB,WGM02);
            }
            else
            {
                return DRV_TIMER_ERROR_INVALID_TOP;
            }
            WRITE_FIELD(TIMER0->TCCRA,WGM00,2,0b11);
            break;

        case DRV_TIMER_MODE_PWM_PHASE_CORRECT:
            if (timer_config->top==DRV_TIMER_TOP_MAX)
            {
                CLEAR_BIT(TIMER0->TCCRB,WGM02);
            }
            else if(timer_config->top==DRV_TIMER_TOP_OCRA)
            {
                SET_BIT(TIMER0->TCCRB,WGM02);
            }
            else
            {
                return DRV_TIMER_ERROR_INVALID_TOP;
            }
            WRITE_FIELD(TIMER0->TCCRA,WGM00,2,0b01);
            break;
        default:
            return DRV_TIMER_ERROR_INVALID_MODE;
        }

        WRITE_FIELD(TIMER0->TCCRA,COM0A0,2,timer_config->OCA_config.output_cmp_mode); 
        WRITE_FIELD(TIMER0->TCCRA,COM0B0,2,timer_config->OCB_config.output_cmp_mode); 
        TIMER0->OCRA=timer_config->OCA_config.output_cmp_value;
        TIMER0->OCRB=timer_config->OCB_config.output_cmp_value;
        TIMER0->TCNT=0;

        if (timer_config->overflow_interrupt_enable){
            timer0_OVF_callback=timer_config->overflow_callback;
            SET_BIT(TIMSK0,TOIE0);
        } 
        else CLEAR_BIT(TIMSK0,TOIE0);
        if (timer_config->OCA_config.interrupt_enable){
            timer0_COMPA_callback=timer_config->OCA_config.callback;
            SET_BIT(TIMSK0,OCIE0A);
        } 
        else CLEAR_BIT(TIMSK0,OCIE0A);
        if (timer_config->OCB_config.interrupt_enable){
            timer0_COMPB_callback=timer_config->OCB_config.callback;
            SET_BIT(TIMSK0,OCIE0B);
        } 
        else CLEAR_BIT(TIMSK0,OCIE0B);
        break;

    case DRV_TIMER_1:
        if (timer_config->OCA_config.output_cmp_mode==DRV_TIMER_OUTPUT_CMP_PIN_TOGGLE && !drv_timer1_oc1a_toggle_supported(timer_config))
        {
            return DRV_TIMER_ERROR_INVALID_OUTPUT_CMP_MODE;
        }
        
        switch (timer_config->timer_mode)
        {
        case DRV_TIMER_MODE_NORMAL:
            WRITE_FIELD(TIMER1->TCCRB, WGM12, 2, 0b00);
            WRITE_FIELD(TIMER1->TCCRA, WGM10, 2, 0b00);
            break;

        case DRV_TIMER_MODE_CTC:
            switch (timer_config->top)
            {
            case DRV_TIMER_TOP_OCRA:
                WRITE_FIELD(TIMER1->TCCRB, WGM12, 2, 0b01);
                WRITE_FIELD(TIMER1->TCCRA, WGM10, 2, 0b00);
                break;

            case DRV_TIMER_TOP_ICR:
                WRITE_FIELD(TIMER1->TCCRB, WGM12, 2, 0b11);
                WRITE_FIELD(TIMER1->TCCRA, WGM10, 2, 0b00);
                break;

            default:
                return DRV_TIMER_ERROR_INVALID_TOP;
            }
            break;

        case DRV_TIMER_MODE_FAST_PWM:
            switch (timer_config->top)
            {
            case DRV_TIMER_TOP_8_BIT:
                WRITE_FIELD(TIMER1->TCCRB, WGM12, 2, 0b01);
                WRITE_FIELD(TIMER1->TCCRA, WGM10, 2, 0b01);
                break;

            case DRV_TIMER_TOP_9_BIT:
                WRITE_FIELD(TIMER1->TCCRB, WGM12, 2, 0b01);
                WRITE_FIELD(TIMER1->TCCRA, WGM10, 2, 0b10);
                break;

            case DRV_TIMER_TOP_10_BIT:
                WRITE_FIELD(TIMER1->TCCRB, WGM12, 2, 0b01);
                WRITE_FIELD(TIMER1->TCCRA, WGM10, 2, 0b11);
                break;

            case DRV_TIMER_TOP_ICR:
                WRITE_FIELD(TIMER1->TCCRB, WGM12, 2, 0b11);
                WRITE_FIELD(TIMER1->TCCRA, WGM10, 2, 0b10);
                break;

            case DRV_TIMER_TOP_OCRA:
                WRITE_FIELD(TIMER1->TCCRB, WGM12, 2, 0b11);
                WRITE_FIELD(TIMER1->TCCRA, WGM10, 2, 0b11);
                break;

            default:
                return DRV_TIMER_ERROR_INVALID_TOP;
            }
            break;

        case DRV_TIMER_MODE_PWM_PHASE_CORRECT:
            switch (timer_config->top)
            {
            case DRV_TIMER_TOP_8_BIT:
                WRITE_FIELD(TIMER1->TCCRB, WGM12, 2, 0b00);
                WRITE_FIELD(TIMER1->TCCRA, WGM10, 2, 0b01);
                break;

            case DRV_TIMER_TOP_9_BIT:
                WRITE_FIELD(TIMER1->TCCRB, WGM12, 2, 0b00);
                WRITE_FIELD(TIMER1->TCCRA, WGM10, 2, 0b10);
                break;

            case DRV_TIMER_TOP_10_BIT:
                WRITE_FIELD(TIMER1->TCCRB, WGM12, 2, 0b00);
                WRITE_FIELD(TIMER1->TCCRA, WGM10, 2, 0b11);
                break;

            case DRV_TIMER_TOP_ICR:
                WRITE_FIELD(TIMER1->TCCRB, WGM12, 2, 0b10);
                WRITE_FIELD(TIMER1->TCCRA, WGM10, 2, 0b10);
                break;

            case DRV_TIMER_TOP_OCRA:
                WRITE_FIELD(TIMER1->TCCRB, WGM12, 2, 0b10);
                WRITE_FIELD(TIMER1->TCCRA, WGM10, 2, 0b11);
                break;

            default:
                return DRV_TIMER_ERROR_INVALID_TOP;
            }
            break;

        case DRV_TIMER_MODE_PWM_PHASE_FREQ_CORRECT:
            switch (timer_config->top)
            {
            case DRV_TIMER_TOP_ICR:
                WRITE_FIELD(TIMER1->TCCRB, WGM12, 2, 0b10);
                WRITE_FIELD(TIMER1->TCCRA, WGM10, 2, 0b00);
                break;

            case DRV_TIMER_TOP_OCRA:
                WRITE_FIELD(TIMER1->TCCRB, WGM12, 2, 0b10);
                WRITE_FIELD(TIMER1->TCCRA, WGM10, 2, 0b01);
                break;

            default:
                return DRV_TIMER_ERROR_INVALID_TOP;
            }
            break;

        default:
            return DRV_TIMER_ERROR_INVALID_MODE;
        }
        WRITE_FIELD(TIMER1->TCCRA,COM1A0,2,timer_config->OCA_config.output_cmp_mode); 
        WRITE_FIELD(TIMER1->TCCRA,COM1B0,2,timer_config->OCB_config.output_cmp_mode);
        TIMER1->OCRA=timer_config->OCA_config.output_cmp_value;
        TIMER1->OCRB=timer_config->OCB_config.output_cmp_value;
        TIMER1->ICR=timer_config->input_capture_config.input_capture_reg_value;
        TIMER1->TCNT=0;

        if (timer_config->input_capture_config.noise_canceler_enable){
            SET_BIT(TCCR1B,ICNC1);
        } 
        else CLEAR_BIT(TCCR1B,ICNC1);

        if (timer_config->input_capture_config.capture_edge_select){
            SET_BIT(TCCR1B,ICES1);
        } 
        else CLEAR_BIT(TCCR1B,ICES1);

        if (timer_config->overflow_interrupt_enable){
            timer1_OVF_callback=timer_config->overflow_callback;
            SET_BIT(TIMSK1,TOIE1);
        } 
        else CLEAR_BIT(TIMSK1,TOIE1);
        if (timer_config->OCA_config.interrupt_enable){
            timer1_COMPA_callback=timer_config->OCA_config.callback;
            SET_BIT(TIMSK1,OCIE1A);
        } 
        else CLEAR_BIT(TIMSK1,OCIE1A);
        if (timer_config->OCB_config.interrupt_enable){
            timer1_COMPB_callback=timer_config->OCB_config.callback;
            SET_BIT(TIMSK1,OCIE1B);
        } 
        else CLEAR_BIT(TIMSK1,OCIE1B);
        if (timer_config->input_capture_config.interrupt_enable){
            timer1_CAPT_callback=timer_config->input_capture_config.callback;
            SET_BIT(TIMSK1,ICIE1);
        } 
        else CLEAR_BIT(TIMSK1,ICIE1);
        
        break;

    case DRV_TIMER_2:
        switch (timer_config->timer2_clock_mode)
        {
        case DRV_TIMER_2_CLK_MODE_SYNC:
            CLEAR_BIT(ASSR,EXCLK);
            CLEAR_BIT(ASSR,AS2);
            break;
        case DRV_TIMER_2_CLK_MODE_ASYNC_CRYSTAL:
            CLEAR_BIT(ASSR,EXCLK);
            SET_BIT(ASSR,AS2);
            break;
        
        case DRV_TIMER_2_CLK_MODE_ASYNC_EXT_CLK:
            SET_BIT(ASSR,EXCLK);
            SET_BIT(ASSR,AS2);
            break;
        
        default:
            return DRV_TIMER_ERROR_INVALID_TIMER_2_CLK_MODE;
        }
        switch (timer_config->timer_mode)
        {
        case DRV_TIMER_MODE_NORMAL:
            CLEAR_BIT(TIMER2->TCCRB,WGM22);
            WRITE_FIELD(TIMER2->TCCRA,WGM20,2,0b00);
            break;
        case DRV_TIMER_MODE_CTC:
            if (timer_config->top!=DRV_TIMER_TOP_OCRA)
            {
                return DRV_TIMER_ERROR_INVALID_TOP;
            }
            CLEAR_BIT(TIMER2->TCCRB,WGM22);
            WRITE_FIELD(TIMER2->TCCRA,WGM20,2,0b10);
            break;

        case DRV_TIMER_MODE_FAST_PWM:
            if (timer_config->top==DRV_TIMER_TOP_MAX)
            {
                CLEAR_BIT(TIMER2->TCCRB,WGM22);
            }
            else if(timer_config->top==DRV_TIMER_TOP_OCRA)
            {
                SET_BIT(TIMER2->TCCRB,WGM22);
            }
            else
            {
                return DRV_TIMER_ERROR_INVALID_TOP;
            }
            WRITE_FIELD(TIMER2->TCCRA,WGM20,2,0b11);
            break;

        case DRV_TIMER_MODE_PWM_PHASE_CORRECT:
            if (timer_config->top==DRV_TIMER_TOP_MAX)
            {
                CLEAR_BIT(TIMER2->TCCRB,WGM22);
            }
            else if(timer_config->top==DRV_TIMER_TOP_OCRA)
            {
                SET_BIT(TIMER2->TCCRB,WGM22);
            }
            else
            {
                return DRV_TIMER_ERROR_INVALID_TOP;
            }
            WRITE_FIELD(TIMER2->TCCRA,WGM20,2,0b01);
            break;
        
        default:
            return DRV_TIMER_ERROR_INVALID_MODE;
        }

        WRITE_FIELD(TIMER2->TCCRA,COM2A0,2,timer_config->OCA_config.output_cmp_mode); 
        WRITE_FIELD(TIMER2->TCCRA,COM2B0,2,timer_config->OCB_config.output_cmp_mode); 
        TIMER2->OCRA=timer_config->OCA_config.output_cmp_value;
        TIMER2->OCRB=timer_config->OCB_config.output_cmp_value;
        TIMER2->TCNT=0;

        if (timer_config->overflow_interrupt_enable){
            timer2_OVF_callback=timer_config->overflow_callback;
            SET_BIT(TIMSK2,TOIE2);
        } 
        else CLEAR_BIT(TIMSK2,TOIE2);
        if (timer_config->OCA_config.interrupt_enable){
            timer2_COMPA_callback=timer_config->OCA_config.callback;
            SET_BIT(TIMSK2,OCIE2A);
        } 
        else CLEAR_BIT(TIMSK2,OCIE2A);
        if (timer_config->OCB_config.interrupt_enable){
            timer2_COMPB_callback=timer_config->OCB_config.callback;
            SET_BIT(TIMSK2,OCIE2B);
        } 
        else CLEAR_BIT(TIMSK2,OCIE2B);
        while (READ_FIELD(ASSR,TCR2BUB,5))
        {
            _NOP();
        }
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
        WRITE_FIELD(TIMSK0,0,8,0X00);
        memset(TIMER0,0,sizeof(*TIMER0));
        timer0_OVF_callback=NULL;
        timer0_COMPB_callback=NULL;
        timer0_COMPA_callback=NULL;
        break;
    case DRV_TIMER_1:
        WRITE_FIELD(TIMSK1,0,8,0X00);
        memset(TIMER1,0,sizeof(*TIMER1));
        timer1_OVF_callback=NULL;
        timer1_COMPB_callback=NULL;
        timer1_COMPA_callback=NULL;
        timer1_CAPT_callback=NULL;
        break;
    case DRV_TIMER_2:
        WRITE_FIELD(TIMSK2,0,8,0X00);
        memset(TIMER2,0,sizeof(*TIMER2));
        timer2_OVF_callback=NULL;
        timer2_COMPB_callback=NULL;
        timer2_COMPA_callback=NULL;
        while (READ_FIELD(ASSR,TCR2BUB,5))
        {
            _NOP();
        }
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
        while (READ_BIT(ASSR,TCR2BUB))
        {
            _NOP();
        }
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
        while (READ_BIT(ASSR,TCR2BUB))
        {
            _NOP();
        }
        
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
            while (READ_BIT(ASSR,OCR2AUB))
            {
                _NOP();
            }
        }
        else if (reg==DRV_OUTPUT_CMP_REG_B)
        {
            TIMER2->OCRB=timer_config->OCB_config.output_cmp_value;
            while (READ_BIT(ASSR,OCR2BUB))
            {
                _NOP();
            }
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
        while (READ_BIT(ASSR,TCR2BUB))
        {
            _NOP();
        }
        break;
    
    default:
        return DRV_TIMER_ERROR_INVALID_DEVICE;
    }
    return DRV_TIMER_SUCCESS;
}