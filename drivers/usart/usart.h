#ifndef USART_H
#define USART_H

#include <stdint.h>
#include <stdbool.h>

typedef enum drv_usart_mode_e{
    DRV_USART_ASYNC_MODE,
    DRV_USART_SYNC_MODE,
    DRV_USART_MASTER_SPI_MODE,
    DRV_USART_MODE_COUNT
}drv_usart_mode_t;

typedef enum drv_usart_parity_mode_e{
    DRV_USART_PARITY_DISABLE_MODE,
    DRV_USART_ODD_PARITY_MODE,
    DRV_USART_EVEN_PARITY_MODE,
    DRV_USART_PARITY_MODE_COUNT
}drv_usart_parity_mode_t;

typedef enum drv_usart_char_size_e{
    DRV_USART_CHAR_SIZE_FIVE,
    DRV_USART_CHAR_SIZE_SIX,
    DRV_USART_CHAR_SIZE_SEVEN,
    DRV_USART_CHAR_SIZE_EIGHT,
    DRV_USART_CHAR_SIZE_NINE,
    DRV_USART_CHAR_SIZE_MAX=DRV_USART_CHAR_SIZE_NINE
}drv_usart_char_size_t;

typedef enum drv_usart_error_e{
    DRV_USART_SUCCESS,
    DRV_USART_ERROR_INVALID_MODE,
    DRV_USART_ERROR_INVALID_PARITY_MODE,
    DRV_USART_ERROR_INVALID_CHAR_SIZE,
    DRV_USART_ERROR_INVALID_BAUD,
}drv_usart_error_t;

typedef struct drv_usart_config_s{
    drv_usart_mode_t mode;
    uint16_t baud_rate;
    drv_usart_char_size_t char_size;
    drv_usart_parity_mode_t parity_mode     :2;
    bool receiver_enable                    :1;
    bool transmitter_enable                 :1;
    bool receive_at_rising_edge             :1; //only applicable for sync mode
    bool two_stop_bits_enable               :1;
    //TODO: implement hardware specific features
    // bool speedx2_enable                     :1;
    // bool multiprocessor_comm_mode_enable    :1;
}drv_usart_config_t;

drv_usart_error_t drv_usart_init(drv_usart_config_t *config);

drv_usart_error_t drv_usart_deinit(void);

drv_usart_error_t drv_usart_transmit(uint8_t data);

drv_usart_error_t drv_usart_receive(uint8_t *data);


#endif
