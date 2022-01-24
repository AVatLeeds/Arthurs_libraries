// © Arthur Vie
// June 2020
//
// AV_ADC.cpp - ADC implementation
//

#define __AVR_ATmega2560__
#include <avr/io.h>
#include <stdint.h>

void configure_ADC()
{
    ADMUX = 0b01000000;
    ADCSRA = 0b10000111;
    ADCSRB = 0b00000000;
}

uint16_t ADC_read(uint8_t pin)
{
    ADCSRB = (pin & 0b00001000) ? (ADCSRB | 0b00001000) : (ADCSRB & 0b11110111);
    uint8_t admux_value = pin & 0b00000111;
    ADMUX &= 0b11100000; // clear the mux bits
    ADMUX |= admux_value; // set new multiplexer value
    ADCSRA |= 0b01000000;
    while (ADCSRA & 0b01000000)
    {
        __asm__ __volatile__("nop");
    }
    uint8_t low_byte = ADCL;
    uint8_t high_byte = ADCH;
    uint16_t value = ((uint16_t)high_byte << 8) | low_byte;
    return value;
}
