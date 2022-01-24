#ifndef AV_USART_H
#define AV_USART_H

// A simple USART library for the ATmega 2560
// Arthur Vie 2020
#include <stdint.h>

void USART_init(uint32_t oscillator_frequency, uint32_t baud_rate);

void USART_transmit_byte(uint8_t data_byte);

void USART_transmit_multi_bytes(uint8_t * data_bytes, uint8_t num_bytes);

void USART_transmit_word16(uint16_t data_word_16);

void serial_send_unsigned16(uint16_t number);

void serial_send_signed16(int16_t number);

uint8_t USART_receive_byte(void);

void USART_receive_multi_bytes(uint8_t * data_bytes, uint8_t num_bytes);

#endif