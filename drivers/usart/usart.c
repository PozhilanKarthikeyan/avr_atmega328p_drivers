#include "usart.h"
#include <avr/io.h>
#include "helper_macros.h"

drv_usart_error_t drv_usart_init(drv_usart_config_t *config){
    if (config->baud_rate==0)
    {
        return DRV_USART_ERROR_INVALID_BAUD;
    }
    
    switch(config->mode){
        case DRV_USART_ASYNC_MODE:
            WRITE_FIELD(UCSR0C,UMSEL00,2,0b00);
            break;
        case DRV_USART_SYNC_MODE:
            WRITE_FIELD(UCSR0C,UMSEL00,2,0b01);
            break;
        case DRV_USART_MASTER_SPI_MODE:
            WRITE_FIELD(UCSR0C,UMSEL00,2,0b11);
            break;
        default:
            return DRV_USART_ERROR_INVALID_MODE;
    }

    switch (config->parity_mode)
    {
    case DRV_USART_PARITY_DISABLE_MODE:
        WRITE_FIELD(UCSR0C,UPM00,2,0b00);
        break;
    case DRV_USART_EVEN_PARITY_MODE:
        WRITE_FIELD(UCSR0C,UPM00,2,0b10);
        break;
    case DRV_USART_ODD_PARITY_MODE:
        WRITE_FIELD(UCSR0C,UPM00,2,0b11);
        break;
    
    default:
        return DRV_USART_ERROR_INVALID_PARITY_MODE;
    }


    switch (config->char_size)
    {
        case DRV_USART_CHAR_SIZE_FIVE:
            WRITE_FIELD(UCSR0C, UCSZ00, 2, 0b00);
            CLEAR_BIT(UCSR0B, UCSZ02);
            break;

        case DRV_USART_CHAR_SIZE_SIX:
            WRITE_FIELD(UCSR0C, UCSZ00, 2, 0b01);
            CLEAR_BIT(UCSR0B, UCSZ02);
            break;

        case DRV_USART_CHAR_SIZE_SEVEN:
            WRITE_FIELD(UCSR0C, UCSZ00, 2, 0b10);
            CLEAR_BIT(UCSR0B, UCSZ02);
            break;

        case DRV_USART_CHAR_SIZE_EIGHT:
            WRITE_FIELD(UCSR0C, UCSZ00, 2, 0b11);
            CLEAR_BIT(UCSR0B, UCSZ02);
            break;

        case DRV_USART_CHAR_SIZE_NINE:
            WRITE_FIELD(UCSR0C, UCSZ00, 2, 0b11);
            SET_BIT(UCSR0B, UCSZ02);
            break;

        default:
            return DRV_USART_ERROR_INVALID_CHAR_SIZE;
    }

    if (config->two_stop_bits_enable)
    {
        SET_BIT(UCSR0C, USBS0);
    }
    else
    {
        CLEAR_BIT(UCSR0C, USBS0);
    }

    if (config->mode == DRV_USART_SYNC_MODE)
    {
        if (config->receive_at_rising_edge)
        {
            SET_BIT(UCSR0C, UCPOL0);
        }
        else
        {
            CLEAR_BIT(UCSR0C, UCPOL0);
        }
    }

    if (config->receiver_enable)
    {
        SET_BIT(UCSR0B, RXEN0);
    }
    else
    {
        CLEAR_BIT(UCSR0B, RXEN0);
    }

    if (config->transmitter_enable)
    {
        SET_BIT(UCSR0B, TXEN0);
    }
    else
    {
        CLEAR_BIT(UCSR0B, TXEN0);
    }


    uint16_t ubrr;
    uint32_t divisor;
    switch (config->mode)
    {
        case DRV_USART_ASYNC_MODE:
            //TODO : Expand this when implementing double speed mode
            
            divisor = 16UL * config->baud_rate;
            break;

        case DRV_USART_SYNC_MODE:
            // divisor = 2UL * baud_rate;
            // break;
        case DRV_USART_MASTER_SPI_MODE:
            divisor= 2UL * config->baud_rate;
            break;

        default:
            return DRV_USART_ERROR_INVALID_MODE;
    }
    ubrr = (uint16_t)((F_CPU + (divisor / 2UL)) / divisor - 1UL); //rounding to nearest integer

    UBRR0H = (ubrr >> 8) & 0XFF;
    UBRR0L = (ubrr) & 0XFF;

    return DRV_USART_SUCCESS;
}

drv_usart_error_t drv_usart_deinit(void){
    CLEAR_BIT(UCSR0B, RXEN0);
    CLEAR_BIT(UCSR0B, TXEN0);

    CLEAR_BIT(UCSR0B, RXCIE0);
    CLEAR_BIT(UCSR0B, TXCIE0);
    CLEAR_BIT(UCSR0B, UDRIE0);

    UCSR0C = 0x06; //UCSZ01:0  = 11  → 8-bit character
    UCSR0A = 0x00;

    UBRR0H = 0x00;
    UBRR0L = 0x00;

    return DRV_USART_SUCCESS;
}

drv_usart_error_t drv_usart_transmit(uint8_t data){
    while (!(READ_BIT(UCSR0A, UDRE0)))
    {
        /* Wait */
    }
    UDR0 = data;

    return DRV_USART_SUCCESS;
}

drv_usart_error_t drv_usart_receive(uint8_t *data){
    while (!(READ_BIT(UCSR0A, RXC0)))
    {
        /* Wait */
    }

    *data = UDR0;

    if (READ_BIT(UCSR0A, FE0))
    {
        return DRV_USART_ERROR_FRAME;
    }

    if (READ_BIT(UCSR0A, DOR0))
    {
        return DRV_USART_ERROR_DATA_OVERRUN;
    }

    if (READ_BIT(UCSR0A, UPE0))
    {
        return DRV_USART_ERROR_PARITY;
    }

    return DRV_USART_SUCCESS;
}

