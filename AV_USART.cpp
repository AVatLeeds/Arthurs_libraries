// A simple USART library for the ATmega 2560
// © Arthur Vie 2020
#define __AVR_ATmega2560__
#include <avr/io.h>
#include <stdint.h>

void USART_init(uint32_t oscillator_frequency, uint32_t baud_rate)
{
    // fill the USART baud rate high and low registers (UBRRnH & UBRRnL)
    uint32_t baud_register_value = (oscillator_frequency / (16 * baud_rate)) - 1;
    //UBRR0H = (unsigned char)(baud_register_value << 8);
    UBRR0L = (uint8_t)baud_register_value;
    // | RXCIEn | TXCIEn | UDRIEn | RXENn | TXENn | UCSZn2 | RXB8n | TXB8n |
    UCSR0B = 0b00001000;
    // | UMSELn0-1 | UPMn0-1 | USBSn | UCSZn0-1 | UCPOLn |
    UCSR0C = 0b000000110;
}

void USART_transmit_byte(uint8_t data_byte)
{
    // while the buffer empty flag in UCSR0A is not set
    while (!(UCSR0A & 0b00100000));
    // populate buffer and send
    UDR0 = data_byte;
}

void USART_transmit_multi_bytes(uint8_t * data_bytes, uint8_t num_bytes)
{
    uint8_t i;
    for (i = 0; i < num_bytes; i++)
    {
        // while the buffer empty flag in UCSR0A is not set
        while (!(UCSR0A & 0b00100000));
        // populate buffer and send
        UDR0 = *data_bytes;
        data_bytes ++;
    } 
}

void USART_transmit_word16(uint16_t data_word_16){
    static uint8_t bytes[2];
    bytes[0] = (uint8_t)(data_word_16 >> 8);
    bytes[1] = (uint8_t)data_word_16;
    USART_transmit_multi_bytes(bytes, 2);
}

void serial_send_unsigned16(uint16_t number)
{
    uint16_t i = 0;
    char temp_buffer[5];
    if (number == 0)
    {
        USART_transmit_byte(48);
        return;
    }
    while (number)
    {
        temp_buffer[i] = 48 + (number % 10);
        number /= 10;
        i ++;
    }
    for (; i >= 1; i --)
    {
        USART_transmit_byte(temp_buffer[i - 1]);
    }
}

void serial_send_signed16(int16_t number)
{
    uint16_t i = 0;
    char temp_buffer[5];
    if (number & 0x8000)
    {
        USART_transmit_byte(45);
        number *= -1;
    }
    if (number == 0)
    {
        USART_transmit_byte(48);
        return;
    }
    while (number)
    {
        temp_buffer[i] = 48 + (number % 10);
        number /= 10;
        i ++;
    }
    for (; i >= 1; i --)
    {
        USART_transmit_byte(temp_buffer[i - 1]);
    }
}

uint8_t USART_receive_byte(void)
{
    // while the USART receive complete flag in UCSR0A is not set
    while (!(UCSR0A & 0b10000000));
    return UDR0;
}

void USART_receive_multi_bytes(uint8_t * data_bytes, uint8_t num_bytes)
{
    uint8_t i;
    for (i = 0; i < num_bytes; i++)
    {
        // while the USART receive complete flag in UCSR0A is not set
        while (!(UCSR0A & 0b10000000));
        *data_bytes = UDR0;
        data_bytes ++;
    }
    
}

