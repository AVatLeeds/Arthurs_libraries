#ifndef AV_ADC_H
#define AV_ADC_H
#include <stdint.h>

#define ADC0 0x0
#define ADC1 0x1
#define ADC2 0x2
#define ADC3 0x3
#define ADC4 0x4
#define ADC5 0x5
#define ADC6 0x6
#define ADC7 0x7
#define ADC8 0x8
#define ADC9 0x9
#define ADC10 0xA
#define ADC11 0xB
#define ADC12 0xC
#define ADC13 0xD
#define ADC14 0xE
#define ADC15 0xF

void configure_ADC(void);

uint16_t ADC_read(uint8_t pin);

#endif // AV_ADC_H
