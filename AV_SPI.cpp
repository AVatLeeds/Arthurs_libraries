#define __AVR_ATmega328P__
#include <avr/io.h>
#include <stdint.h>

void SPI_master_init()
{
    // Set SS (PB2), MOSI (PB3) and SCK (PB5) as outputs
    DDRB |= 0b00101100;
    // Set MISO (PB4) as input
    DDRB &= 0b11101111;
    PORTB |= 0b00000100; // Raise SS
    SPCR = 0b01011100;
}

void SPI_send(uint8_t data_byte)
{
    SPDR = data_byte;
    while (!(SPSR & 0b10000000));
}

uint8_t SPI_receive()
{
    SPDR = 0x00;
    while (!(SPSR & 0b10000000));
    return SPDR;
}

uint8_t SPI_read_register(uint8_t address)
{
    PORTB &= 0b11111011; //Lower SS
    SPI_send(address |= 0b10000000);
    uint8_t received_byte = SPI_receive();
    PORTB |= 0b00000100; // Raise SS
    return received_byte;
}

void SPI_write_register(uint8_t address, uint8_t value)
{
    PORTB &= 0b11111011; //Lower SS
    SPI_send(address &= 0b01111111);
    SPI_send(value);
    PORTB |= 0b00000100; // Raise SS
    return;
}
